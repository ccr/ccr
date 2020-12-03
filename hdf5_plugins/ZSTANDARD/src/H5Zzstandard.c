/* Copyright (C) 2020--present Charlie Zender */

/*
 * This file is an example of an HDF5 filter plugin.
 * The plugin can be used with the HDF5 library vesrion 1.8.11+ to read and write
 * HDF5 datasets compressed with Zstandard
 */

#include "config.h"
#include <stdio.h>
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif
#ifdef STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif
#ifdef HAVE_STRING_H
# if !defined STDC_HEADERS && defined HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#endif
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <assert.h>

#if defined(_WIN32)
#include <Winsock2.h>
#endif

/* 3rd party vendors */
#include "H5PLextern.h" /* HDF5 Plugin Library: H5PLget_plugin_type(), H5PLget_plugin_info() */
#include "zstd.h" /* Zstandard library header */

/* Tokens and typedefs */
#define H5Z_FILTER_ZSTANDARD 32015 /* NB: Registered with HDF */
#define CCR_FLT_DBG_INFO 0 /* [flg] Print non-fatal debugging information */
#define CCR_FLT_HELP "HINT: Read the description of Zstandard compression levels and their speed vs. compression-ratio tradeoffs at http://zstd.net"
#define CCR_FLT_NAME "Zstandard filter for HDF5; http://www.zstd.net" /* [sng] Filter name in vernacular for HDF5 messages */
#define CCR_FLT_PRM_NBR 1 /* [nbr] Number of parameters sent to filter (in cd_params array) */
#define CCR_FLT_PRM_PSN_CMP_LVL 0 /* [nbr] Ordinal position of CMP_LVL in parameter list (cd_params array) */

/* Forward-declare functions before their names appear in H5Z_class2_t filter structure */
size_t /* O [B] Number of bytes processed from input buffer (?) */
H5Z_filter_zstandard /* [fnc] HDF5 Zstandard Filter */
(unsigned int flags, /* I [flg] Bitfield that encodes filter direction */
 size_t cd_nelmts, /* I [nbr] Number of elements in filter parameter (cd_values[]) array */
 const unsigned int cd_values[], /* I [enm] Filter parameters */
 size_t bfr_sz_in, /* I [B] Number of bytes in input buffer (before forward/reverse filter) */
 size_t *bfr_sz_out, /* O [B] Number of bytes in output buffer (after forward/reverse filter) */
 void **bfr_inout); /* I/O [frc] Values to compress */

const H5Z_class2_t H5Z_ZSTANDARD[1]={{
    H5Z_CLASS_T_VERS, /* H5Z_class_t version */
    (H5Z_filter_t)H5Z_FILTER_ZSTANDARD, /* Filter ID number */
#ifdef FILTER_DECODE_ONLY
    0, /* [flg] Encoder availability flag */
#else
    1, /* [flg] Encoder availability flag */
#endif
    1, /* [flg] Encoder availability flag */
    CCR_FLT_NAME, /* [sng] Filter name for debugging */
    NULL, /* [fnc] Callback to determine if current variable meets filter criteria */
    NULL, /* [fnc] Callback to determine and set per-variable filter parameters */
    (H5Z_func_t)H5Z_filter_zstandard, /* [fnc] Function to implement filter */
  }}; /* !H5Z_ZSTANDARD */

/* Function definitions */
H5PL_type_t /* O [enm] Plugin type */
H5PLget_plugin_type /* [fnc] Provide plug-in type provided by this shared library */
(void)
{ /* Purpose: Describe plug-in type provided by this shared library
     The HDF5 plugin mechanism usually calls this function after an application calls H5Pset_filter(), or when the data to which this filter will be applied are first read */
  return H5PL_TYPE_FILTER;
} /* !H5PLget_plugin_type() */

const void * /* O [enm] */
H5PLget_plugin_info /* [fnc] Return structure */
(void)
{ /* Purpose: Provide structure that defines Zstandard filter so the filter may be dynamically registered with the plugin mechanism
     The HDF5 plugin mechanism usually calls this function after an application calls H5Pset_filter(), or when the data to which this filter will be applied are first read */
  return H5Z_ZSTANDARD;
} /* !H5PLget_plugin_info() */

size_t /* O [B] Number of bytes resulting after forward/reverse filter applied */
H5Z_filter_zstandard /* [fnc] HDF5 Zstandard Filter */
(unsigned int flags, /* I [flg] Bitfield that encodes filter direction */
 size_t cd_nelmts, /* I [nbr] Number of elements in filter parameter (cd_values[]) array */
 const unsigned int cd_values[], /* I [enm] Filter parameters */
 size_t bfr_sz_in, /* I [B] Number of bytes in input buffer (before forward/reverse filter) */
 size_t *bfr_sz_out, /* O [B] Number of bytes in output buffer (after forward/reverse filter) */
 void **bfr_inout) /* I/O [frc] Values to compress/decompress */
{
  /* Purpose: Dynamic filter invoked by HDF5 to compress/decompress a variable with Zstandard */

  const char fnc_nm[]="H5Z_filter_zstandard()"; /* [sng] Function name */

  size_t rvl; /* O [B] Return value = number of bytes resulting after forward/reverse filter applied */
  
  void *bfr_in=NULL; /* [ptr] Pointer to input buffer (before forward/reverse filter) */
  void *bfr_out=NULL; /* [ptr] Pointer to output buffer (after forward/reverse filter) */

  /* Save original input buffer */
  bfr_in=*bfr_inout;
  
  /* 20200915 fxm compress/decompress in streaming mode to handle larger buffers? */

  if(flags & H5Z_FLAG_REVERSE){

    size_t dcmp_sz; /* [B] Actual decompressed size of source frame content, if known, otherwise error code */
    size_t dcmp_sz_max; /* [B] Decompressed size of source frame content, if known, otherwise error code */
#ifdef HAVE_ZSTD_GETFRAMECONTENTSIZE
    dcmp_sz_max=ZSTD_getFrameContentSize(*bfr_inout,bfr_sz_in);
#else  /* !HAVE_ZSTD_GETFRAMECONTENTSIZE */
    /* 20200920: Zstandard 1.3.1 distributed with Ubuntu Xenial 16.04 LTS lacks ZSTD_getFrameContentSize() */
    dcmp_sz_max=ZSTD_getDecompressedSize(*bfr_inout,bfr_sz_in);
#endif  /* !HAVE_ZSTD_GETFRAMECONTENTSIZE */
    if(ZSTD_isError(dcmp_sz_max)){
      (void)fprintf(stderr,"ERROR: \"%s\" filter function %s reports error return code = %lu from ZSTD_getFrameContentSize()\n",CCR_FLT_NAME,fnc_nm,dcmp_sz_max);
      goto error;
    } /* !dcmp_sz_max */

    if(!(bfr_out=malloc(dcmp_sz_max))){
      (void)fprintf(stderr,"ERROR: \"%s\" filter function %s reports failure to malloc dcmp_sz_max = %lu B\n",CCR_FLT_NAME,fnc_nm,dcmp_sz_max);
      goto error;
    } /* !bfr_out */

    /* Decompress the input buffer */
    dcmp_sz=ZSTD_decompress(bfr_out,dcmp_sz_max,bfr_in,bfr_sz_in);
    if(ZSTD_isError(dcmp_sz)){
      (void)fprintf(stderr,"ERROR: \"%s\" filter function %s reports error return code = %lu from ZSTD_decompress()\n",CCR_FLT_NAME,fnc_nm,dcmp_sz);
      goto error;
    } /* !dcmp_sz */
    /* Return number of bytes in compressed buffer */
    rvl=dcmp_sz;
    
  }else{ /* !flags */
    
    /* Set parameters needed by compression library filter */
#ifdef HAVE_ZSTD_MINCLEVEL
    /* Reported by modern (~v. 1.4.5, ~2020) Zstandard libraries, not distributed by Ubuntu Bionic */
   const int cmp_lvl_min=ZSTD_minCLevel(); /* [enm] Minimum compression aggression level */
#else /* !HAVE_ZSTD_MINCLEVEL */
   /* 20200920: Zstandard distributed with Ubuntu Bionic Travis CI build environment ZSTD_minCLevel() */
   /* 20200920: Zstandard 1.3.1 distributed with Ubuntu Xenial 16.04 LTS lacks ZSTD_minCLevel() */
   const int cmp_lvl_min=1; /* [enm] Minimum compression aggression level */
#endif /* !HAVE_ZSTD_MINCLEVEL */
#ifdef HAVE_ZSTD_MAXCLEVEL
    const int cmp_lvl_max=ZSTD_maxCLevel(); /* [enm] Maximum compression aggression level */
#else /* !HAVE_ZSTD_MAXCLEVEL */
    /* 20200920: Zstandard 0.5 distributed with Ubuntu Xenial 16.04 LTS lacks ZSTD_maxCLevel() */
    const int cmp_lvl_max=19; /* [enm] Maximum compression aggression level (supported by v. 1.3.1)*/
#endif /* !HAVE_ZSTD_MAXCLEVEL */

    int cmp_lvl; /* [enm] Compression aggression level */

    /* 20200915: Zstandard defines ZSTD_CLEVEL_DEFAULT == 3 since version 0.7.4 (~2016)
       However, earlier zstd.h may not define this token, as evidenced by failure of 
       Travis CI on Ubuntu Bionic to find this token with that version of Zstandard */
#ifndef ZSTD_CLEVEL_DEFAULT
# define ZSTD_CLEVEL_DEFAULT 3
#endif

    /* NB: <zstd.h> sets ZSTD_CLEVEL_DEFAULT == 3 */
    if(cd_nelmts > 0) cmp_lvl=(int)cd_values[CCR_FLT_PRM_PSN_CMP_LVL]; else cmp_lvl=ZSTD_CLEVEL_DEFAULT;
    if(cmp_lvl < cmp_lvl_min){
      (void)fprintf(stderr,"WARNING: \"%s\" filter function %s must adjust actual compression level from user-requested value of %d to minimum Zstandard-supported compression level = %d\n",CCR_FLT_NAME,fnc_nm,cmp_lvl,cmp_lvl_min);
      (void)fprintf(stderr,"%s\n",CCR_FLT_HELP);
      cmp_lvl=cmp_lvl_min;
    } /* !cmp_lvl */
    if(cmp_lvl > cmp_lvl_max){
      (void)fprintf(stderr,"WARNING: \"%s\" filter function %s must adjust actual compression level from user-requested value of %d to maximum Zstandard-supported compression level = %d\n",CCR_FLT_NAME,fnc_nm,cmp_lvl,cmp_lvl_max);
      (void)fprintf(stderr,"%s\n",CCR_FLT_HELP);
      cmp_lvl=cmp_lvl_max;
    } /* !cmp_lvl */
    
    if(CCR_FLT_DBG_INFO) (void)fprintf(stderr,"INFO: %s reports cmp_lvl = %d, cmp_lvl_min = %d, cmp_lvl_max = %d\n",fnc_nm,cmp_lvl,cmp_lvl_min,cmp_lvl_max);
    
    size_t cmp_sz; /* [B] Compressed size written into output buffer (or error code) */
    size_t cmp_sz_max; /* [B] Maximum compressed size in worst case single-pass scenario */
    cmp_sz_max=ZSTD_compressBound(bfr_sz_in);
    if(ZSTD_isError(cmp_sz_max)){
      (void)fprintf(stderr,"ERROR: \"%s\" filter function %s reports error return code = %lu from ZSTD_compressBound()\n",CCR_FLT_NAME,fnc_nm,cmp_sz_max);
      goto error;
    } /* !cmp_sz_max */

    if(!(bfr_out=malloc(cmp_sz_max))){
      (void)fprintf(stderr,"ERROR: \"%s\" filter function %s reports failure to malloc cmp_sz_max = %lu B\n",CCR_FLT_NAME,fnc_nm,cmp_sz_max);
      goto error;
    } /* !bfr_out */
    
    /* Compress the input buffer */
    cmp_sz=ZSTD_compress(bfr_out,cmp_sz_max,bfr_in,bfr_sz_in,cmp_lvl);
    if(ZSTD_isError(cmp_sz)){
      (void)fprintf(stderr,"ERROR: \"%s\" filter function %s reports error return code = %lu from ZSTD_compress()\n",CCR_FLT_NAME,fnc_nm,cmp_sz);
      goto error;
    } /* !cmp_sz */
    
    /* Return number of bytes in compressed buffer */
    rvl=cmp_sz;
    
  } /* !flags */
  
  free(*bfr_inout);
  *bfr_inout=bfr_out;
  *bfr_sz_out=rvl;
  bfr_out=NULL;
  return rvl;
  
 error:
  /* Compression filter failed, so free any allocated output buffer and return with error code */
  if(bfr_out) free(bfr_out);
  return 0;

} /* !H5Z_filter_zstandard() */
