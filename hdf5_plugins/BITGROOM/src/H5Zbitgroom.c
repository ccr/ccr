/* Copyright (C) 2020--present Charlie Zender */

 /*
 * This file is an example of an HDF5 filter plugin.
 * The plugin can be used with the HDF5 library vesrion 1.8.11+ to read and write
 * HDF5 datasets quantized with BitGrooming.
 */

#ifdef HAVE_CONFIG_H
# include "config.h" /* Autotools tokens */
#endif
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

/* Standard header files */
#ifdef HAVE_FEATURES_H
/* Needed to define __USE_BSD that recent GCC compilers use in math.h to define M_LN2... */
# include <features.h> /* __USE_BSD */
#endif
#ifdef HAVE_MATH_H
/* Needed for M_LN10, M_LN2 in ccr_bgr() */
# include <math.h> /* sin cos cos sin 3.14159 */
#endif

/* 3rd party vendors */
#include "H5PLextern.h" /* HDF5 Plugin Library: H5PLget_plugin_type(), H5PLget_plugin_info() */

/* Tokens and typedefs */
#define H5Z_FILTER_BITGROOM 32022 /* NB: ID assigned by HDF Group 20201223 */
#define CCR_FLT_DBG_INFO 0 /* [flg] Print non-fatal debugging information */
#define CCR_FLT_NAME "BitGroom filter (Zender, 2016 GMD: http://www.geosci-model-dev.net/9/3199/2016)" /* [sng] Filter name in vernacular for HDF5 messages */
#define CCR_FLT_NSD_DFL 3 /* [nbr] Default number of significant digits for quantization */
#define CCR_FLT_PRM_NBR 5 /* [nbr] Number of parameters sent to filter (in cd_params array). NB: keep identical with ccr.h:BITGROOM_FLT_PRM_NBR */
#define CCR_FLT_PRM_PSN_NSD 0 /* [nbr] Ordinal position of NSD in parameter list (cd_params array) */
#define CCR_FLT_PRM_PSN_DATUM_SIZE 1 /* [nbr] Ordinal position of datum_size in parameter list (cd_params array) */
#define CCR_FLT_PRM_PSN_HAS_MSS_VAL 2 /* [nbr] Ordinal position of missing value flag in parameter list (cd_params array) */
#define CCR_FLT_PRM_PSN_MSS_VAL 3 /* [nbr] Ordinal position of missing value in parameter list (cd_params array) NB: Missing value (_FillValue) uses two cd_params slots so it can be single or double-precision. Single-precision values are read as first 4-bytes starting at cd_params[4] (and cd_params[5] is ignored), while double-precision values are read as first 8-bytes starting at cd_params[4] and ending with cd_params[5]. */

/* Compatibility tokens and typedefs retain source-code compatibility between NCO and filter
   These tokens mimic netCDF/NCO code but do not rely on or interfere with either */
#ifndef NC_FLOAT
# define NC_FLOAT 5
#endif /* !NC_FLOAT */
#ifndef NC_DOUBLE
# define NC_DOUBLE 6
#endif /* !NC_DOUBLE */

/* Minimum number of explicit significand bits to preserve when zeroing/bit-masking floating point values
   Codes will preserve at least two explicit bits, IEEE significand representation contains one implicit bit
   Thus preserve a least three bits which is approximately one sigificant decimal digit
   Used in nco_ppc_bitmask() and nco_ppc_bitmask_scl() */
#define NCO_PPC_BIT_XPL_NBR_MIN 2

/* Pointer union for floating point and bitmask types */
typedef union{ /* ptr_unn */
  float *fp;
  double *dp;
  unsigned int *ui32p;
  unsigned long long *ui64p;
  void *vp;
} ptr_unn;

/* Forward-declare functions before their names appear in H5Z_class2_t filter structure */
size_t /* O [B] Number of bytes resulting after forward/reverse filter applied */
H5Z_filter_bitgroom /* [fnc] HDF5 BitGroom Filter */
(unsigned int flags, /* I [flg] Bitfield that encodes filter direction */
 size_t cd_nelmts, /* I [nbr] Number of elements in filter parameter (cd_values[]) array */
 const unsigned int cd_values[], /* I [enm] Filter parameters */
 size_t bfr_sz_in, /* I [B] Number of bytes in input buffer (before forward/reverse filter) */
 size_t *bfr_sz_out, /* O [B] Number of bytes in output buffer (after forward/reverse filter) */
 void **bfr_inout); /* I/O [frc] Values to quantize */

htri_t /* O [flg] Data meet criteria to apply filter */
ccr_can_apply_bitgroom /* [fnc] Callback to determine if current variable meets filter criteria */
(hid_t dcpl, /* I [id] Dataset creation property list ID */
 hid_t type, /* I [id] Dataset type ID */
 hid_t space); /* I [id] Dataset space ID */

htri_t /* O [flg] Filter parameters successfully modified for this variable */
ccr_set_local_bitgroom /* [fnc] Callback to determine and set per-variable filter parameters */
(hid_t dcpl, /* I [id] Dataset creation property list ID */
 hid_t type, /* I [id] Dataset type ID */
 hid_t space); /* I [id] Dataset space ID */

const H5Z_class2_t H5Z_BITGROOM[1]={{
    H5Z_CLASS_T_VERS, /* H5Z_class_t version */
    (H5Z_filter_t)H5Z_FILTER_BITGROOM, /* Filter ID number */
#ifdef FILTER_DECODE_ONLY
    0, /* [flg] Encoder availability flag */
#else
    1, /* [flg] Encoder availability flag */
#endif
    1, /* [flg] Encoder availability flag */
    CCR_FLT_NAME, /* [sng] Filter name for debugging */
    ccr_can_apply_bitgroom, /* [fnc] Callback to determine if current variable meets filter criteria */
    ccr_set_local_bitgroom, /* [fnc] Callback to determine and set per-variable filter parameters */
    (H5Z_func_t)H5Z_filter_bitgroom, /* [fnc] Function to implement filter */
  }}; /* !H5Z_BITGROOM */

void
ccr_bgr /* [fnc] BitGroom buffer of float values */
(const int nsd, /* I [nbr] Number of decimal significant digits to quantize to */
 const int type, /* I [enm] netCDF type of operand */
 const size_t sz, /* I [nbr] Size (in elements) of buffer to quantize */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 ptr_unn op1); /* I/O [frc] Values to quantize */

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
{ /* Purpose: Provide structure that defines BitGroom filter so the filter may be dynamically registered with the plugin mechanism
     The HDF5 plugin mechanism usually calls this function after an application calls H5Pset_filter(), or when the data to which this filter will be applied are first read */
  return H5Z_BITGROOM;
} /* !H5PLget_plugin_info() */

size_t /* O [B] Number of bytes resulting after forward/reverse filter applied */
H5Z_filter_bitgroom /* [fnc] HDF5 BitGroom Filter */
(unsigned int flags, /* I [flg] Bitfield that encodes filter direction */
 size_t cd_nelmts, /* I [nbr] Number of elements in filter parameter (cd_values[]) array */
 const unsigned int cd_values[], /* I [enm] Filter parameters */
 size_t bfr_sz_in, /* I [B] Number of bytes in input buffer (before forward/reverse filter) */
 size_t *bfr_sz_out, /* O [B] Number of bytes in output buffer (after forward/reverse filter) */
 void **bfr_inout) /* I/O [frc] Values to quantize */
{
  /* Purpose: Dynamic filter invoked by HDF5 to BitGroom a variable */

  const char fnc_nm[]="H5Z_filter_bitgroom()"; /* [sng] Function name */

  if(flags & H5Z_FLAG_REVERSE){

    /* Currently supported quantization methods (BitGrooming) store results in IEEE754 format 
       These quantized buffers are full of legal IEEE754 numbers that need no "dequantization"
       In other words, the input values in bfr_inout are also the output values */
    return bfr_sz_in;

  }else{ /* !flags */

    /* Set parameters needed by quantization library filter */
    int nsd=cd_values[CCR_FLT_PRM_PSN_NSD];
    size_t datum_size=cd_values[CCR_FLT_PRM_PSN_DATUM_SIZE];
    int has_mss_val=cd_values[CCR_FLT_PRM_PSN_HAS_MSS_VAL]; /* [flg] Flag for missing values */
    ptr_unn mss_val; /* [val] Value of missing value */
    ptr_unn op1; /* I/O [frc] Values to quantize */

    /* ISO C, including gcc -pedantic, forbids casting unions (like mss_val) to incompatible data-types (e.g., NULL, which is void *) so, instead, initialize union member to NULL */
    mss_val.vp=NULL;
    
    if(CCR_FLT_DBG_INFO) (void)fprintf(stderr,"INFO: %s reports datum size = %lu B, has_mss_val = %d\n",fnc_nm,datum_size,has_mss_val);

    /* Quantization is only for floating-point data (data_class == H5T_FLOAT)
       Following block assumes all bfr_inout values are either 4-byte or 8-byte floating point */
    switch(datum_size){
      /* Cast input buffer pointer to correct numeric type */
    case 4:
      /* Single-precision floating-point data */
      if(has_mss_val){
	mss_val.fp=(float *)(cd_values+CCR_FLT_PRM_PSN_MSS_VAL);
	if(CCR_FLT_DBG_INFO) (void)fprintf(stderr,"INFO: \"%s\" filter function %s reports missing value = %g\n",CCR_FLT_NAME,fnc_nm,*mss_val.fp);
      } /* !has_mss_val */
      op1.fp=(float *)(*bfr_inout);
      ccr_bgr(nsd,NC_FLOAT,bfr_sz_in/sizeof(float),has_mss_val,mss_val,op1);
      break;
    case 8:
      /* Double-precision floating-point data */
      if(has_mss_val){
	mss_val.dp=(double *)(cd_values+CCR_FLT_PRM_PSN_MSS_VAL);
	if(CCR_FLT_DBG_INFO) (void)fprintf(stderr,"INFO: \"%s\" filter function %s reports missing value = %g\n",CCR_FLT_NAME,fnc_nm,*mss_val.dp);
      } /* !has_mss_val */
      op1.dp=(double *)(*bfr_inout);
      ccr_bgr(nsd,NC_DOUBLE,bfr_sz_in/sizeof(double),has_mss_val,mss_val,op1);
      break;
    default:
      (void)fprintf(stderr,"ERROR: \"%s\" filter function %s reports datum size = %lu B is invalid\n",CCR_FLT_NAME,fnc_nm,datum_size);
      goto error;
      break;
    } /* !datum_size */
    
    return bfr_sz_in;
    
  } /* !flags */
  
  return 1;
  
 error:
  /* Quantization filters generally allocate no memory, so just return with error code */
  return 0;

} /* !H5Z_filter_bitgroom() */

htri_t /* O [flg] Data meet criteria to apply filter */
ccr_can_apply_bitgroom /* [fnc] Callback to determine if current variable meets filter criteria */
(hid_t dcpl, /* I [id] Dataset creation property list ID */
 hid_t type, /* I [id] Dataset type ID */
 hid_t space) /* I [id] Dataset space ID */
{
  /* Data space must be simple, i.e., a multi-dimensional array */
  if(H5Sis_simple(space) <= 0){
    fprintf(stderr,"WARNING: Cannot apply filter \"%s\" filter because data space is not simple.\n",CCR_FLT_NAME);
    return 0;
  } /* !H5Sis_simple(space) */

  /* Filter can be applied */
  return 1;
} /* !ccr_can_apply_bitgroom() */

htri_t /* O [flg] Filter parameters successfully modified for this variable */
ccr_set_local_bitgroom /* [fnc] Callback to determine and set per-variable filter parameters */
(hid_t dcpl, /* I [id] Dataset creation property list ID */
 hid_t type, /* I [id] Dataset type ID */
 hid_t space) /* I [id] Dataset space ID */
{
  const char fnc_nm[]="ccr_set_local_bitgroom()"; /* [sng] Function name */

  herr_t rcd; /* [flg] Return code */
  
  /* Initialize filter parameters with default values */
  unsigned int ccr_flt_prm[CCR_FLT_PRM_NBR]={CCR_FLT_NSD_DFL,0,0,0,0};

  /* Initialize output variables for call to H5Pget_filter_by_id() */
  unsigned int flags=0;
  size_t cd_nelmts=CCR_FLT_PRM_NBR;
  unsigned int *cd_values=ccr_flt_prm;

  /* Retrieve parameters specified by user
     https://support.hdfgroup.org/HDF5/doc/RM/RM_H5P.html#FunctionIndex
     Ignore name and filter_config by setting last three arguments to 0/NULL */
  rcd=H5Pget_filter_by_id(dcpl,H5Z_FILTER_BITGROOM,&flags,&cd_nelmts,cd_values,0,NULL,NULL);
  if(rcd < 0){
    (void)fprintf(stderr,"ERROR: %s filter callback function %s reports H5Pget_filter_by_id() failed to get filter flags and parameters for current variable\n",CCR_FLT_NAME,fnc_nm);
    return 0;
  } /* !rcd */

  /* Data class for this variable */
  H5T_class_t data_class; /* [enm] Data type class identifier (H5T_FLOAT, H5T_INT, H5T_STRING, ...) */
  data_class=H5Tget_class(type); 
  if(data_class < 0){
    (void)fprintf(stderr,"ERROR: %s filter callback function %s reports H5Tget_class() returned invalid data type class identifier = %d for current variable\n",CCR_FLT_NAME,fnc_nm,(int)data_class);
    return 0;
  }else if(data_class != H5T_FLOAT){
    if(CCR_FLT_DBG_INFO){
      (void)fprintf(stdout,"INFO: \"%s\" filter callback function %s reports H5Tget_class() returned data type class identifier = %d != H5T_FLOAT = %d. Attempting to remove quantization filter using H5Premove_filter()...",CCR_FLT_NAME,fnc_nm,(int)data_class,H5T_FLOAT);
    } /* !CCR_FLT_DBG_INFO */
    rcd=H5Premove_filter(dcpl,H5Z_FILTER_BITGROOM);
    if(rcd < 0){
      if(CCR_FLT_DBG_INFO) (void)fprintf(stdout,"failure :(\n");
      return 0;
    } /* !rcd */
    if(CCR_FLT_DBG_INFO) (void)fprintf(stdout,"success!\n");
    return 1;
  } /* !data_class */
  
  /* Set data class in filter parameter list 
     20200921: Remove data_class from filter parameter list as it is not needed */
  //ccr_flt_prm[CCR_FLT_PRM_PSN_DATA_CLASS]=(unsigned int)data_class;

  /* Datum size for this variable */
  size_t datum_size; /* [B] Bytes per data value */
  datum_size=H5Tget_size(type);
  if(datum_size <= 0){
    (void)fprintf(stderr,"ERROR: %s filter callback function %s reports H5Tget_size() returned invalid datum size = %lu B\n",CCR_FLT_NAME,fnc_nm,datum_size);
    return 0;
  } /* !datum_size */
  /* Set datum size in filter parameter list */
  ccr_flt_prm[CCR_FLT_PRM_PSN_DATUM_SIZE]=(unsigned int)datum_size;

  /* Which variable is this? fxm find and add variable name to debugging info */

  /* Find, set, and pass per-variable has_mss_val and mss_val arguments
     https://support.hdfgroup.org/HDF5/doc_resource/H5Fill_Values.html */
  int has_mss_val=0; /* [flg] Flag for missing values */

  H5D_fill_value_t status;
  rcd=H5Pfill_value_defined(dcpl,&status);
  if(rcd < 0){
    (void)fprintf(stdout,"ERROR: \"%s\" filter callback function %s reports H5Pfill_value_defined() returns error code = %d\n",CCR_FLT_NAME,fnc_nm,rcd);
    return 0;
  } /* !rcd */

  if(CCR_FLT_DBG_INFO){
    (void)fprintf(stdout,"INFO: \"%s\" filter callback function %s reports H5Pfill_value_defined() status = %d meaning ... ",CCR_FLT_NAME,fnc_nm,status);
    if(status == H5D_FILL_VALUE_UNDEFINED) (void)fprintf(stdout,"Fill-value is undefined\n");    
    else if(status == H5D_FILL_VALUE_DEFAULT) (void)fprintf(stdout,"Fill-value is the library default\n");
    else if(status == H5D_FILL_VALUE_USER_DEFINED) (void)fprintf(stdout,"Fill-value is defined by user application\n");
  } /* !CCR_FLT_DBG_INFO */

  ptr_unn mss_val; /* [val] Value of missing value */
  mss_val.vp=NULL;
  if(status == H5D_FILL_VALUE_USER_DEFINED && data_class == H5T_FLOAT){
    unsigned int *ui32p; /* [ptr] Pointer to missing value */

    has_mss_val=1;
    mss_val.vp=(void *)malloc(datum_size);
    rcd=H5Pget_fill_value(dcpl,type,mss_val.vp);
    if(rcd < 0){
      (void)fprintf(stdout,"ERROR: \"%s\" filter callback function %s reports H5Pget_fill_value() returns error code = %d\n",CCR_FLT_NAME,fnc_nm,rcd);
      return 0;
    } /* !rcd */

    /* Set missing value in filter parameter list */
    if(datum_size == 4){
      if(CCR_FLT_DBG_INFO) (void)fprintf(stderr,"INFO: \"%s\" filter callback function %s reports missing value = %g\n",CCR_FLT_NAME,fnc_nm,*mss_val.fp);
      ui32p=(unsigned int *)mss_val.fp;
      cd_values[CCR_FLT_PRM_PSN_MSS_VAL]=*ui32p;
    }else if(datum_size == 8){
      if(CCR_FLT_DBG_INFO) (void)fprintf(stderr,"INFO: \"%s\" filter callback function %s reports missing value = %g\n",CCR_FLT_NAME,fnc_nm,*mss_val.dp);
      ui32p=(unsigned int *)mss_val.dp;
      /* Copy first four-bytes of missing value into unsigned int parameter */
      cd_values[CCR_FLT_PRM_PSN_MSS_VAL]=*ui32p;
      /* Copy second four-bytes of missing value into next unsigned int parameter */
      cd_values[CCR_FLT_PRM_PSN_MSS_VAL+1L]=*(ui32p+1);
    } /* !datum_size */

    /* Free fill value memory */
    if(mss_val.vp) free(mss_val.vp);
  } /* !status */

  /* Set missing value flag in filter parameter list */
  ccr_flt_prm[CCR_FLT_PRM_PSN_HAS_MSS_VAL]=has_mss_val;

  /* Update invoked filter with generic parameters as invoked with variable-specific values */
  rcd=H5Pmodify_filter(dcpl,H5Z_FILTER_BITGROOM,flags,CCR_FLT_PRM_NBR,cd_values);
  if(rcd < 0){
    (void)fprintf(stderr,"ERROR: \"%s\" filter callback function %s reports H5Pmodify_filter() unable to modify filter parameters\n",CCR_FLT_NAME,fnc_nm);
    return 0;
  } /* !rcd */

  return 1;
} /* !ccr_set_local_bitgroom() */

void
ccr_bgr /* [fnc] BitGroom buffer of float values */
(const int nsd, /* I [nbr] Number of decimal significant digits to quantize to */
 const int type, /* I [enm] netCDF type of operand */
 const size_t sz, /* I [nbr] Size (in elements) of buffer to quantize */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 ptr_unn op1) /* I/O [frc] Values to quantize */
{
  const char fnc_nm[]="ccr_bgr()"; /* [sng] Function name */

  /* Prefer constants defined in math.h, however, ...
     20201002 GCC environments can have hard time defining M_LN10/M_LN2 despite finding math.h */
#ifndef M_LN10
# define M_LN10         2.30258509299404568402  /* log_e 10 */
#endif /* M_LN10 */
#ifndef M_LN2
# define M_LN2          0.69314718055994530942  /* log_e 2 */
#endif /* M_LN2 */
  const double bit_per_dcm_dgt_prc=M_LN10/M_LN2; /* 3.32 [frc] Bits per decimal digit of precision */
  //const double dcm_per_bit_dgt_prc=M_LN2/M_LN10; /* 0.301 [frc] Bits per decimal digit of precision */
  
  const int bit_xpl_nbr_sgn_flt=23; /* [nbr] Bits 0-22 of SP significands are explicit. Bit 23 is implicitly 1. */
  const int bit_xpl_nbr_sgn_dbl=53; /* [nbr] Bits 0-52 of DP significands are explicit. Bit 53 is implicitly 1. */
  //const int ieee_xpn_fst_flt=127; /* [nbr] IEEE "exponent bias" = actual exponent minus stored exponent */
  
  double prc_bnr_xct; /* [nbr] Binary digits of precision, exact */
  
  int bit_xpl_nbr_sgn=-1; /* [nbr] Number of explicit bits in significand */
  int bit_xpl_nbr_zro; /* [nbr] Number of explicit bits to zero */

  size_t idx;

  unsigned int *u32_ptr;
  unsigned int msk_f32_u32_zro;
  unsigned int msk_f32_u32_one;
  //unsigned int msk_f32_u32_hshv;
  unsigned long long int *u64_ptr;
  unsigned long long int msk_f64_u64_zro;
  unsigned long long int msk_f64_u64_one;
  //unsigned long long int msk_f64_u64_hshv;
  unsigned short prc_bnr_ceil; /* [nbr] Exact binary digits of precision rounded-up */
  unsigned short prc_bnr_xpl_rqr; /* [nbr] Explicitly represented binary digits required to retain */

  /* Disallow unreasonable quantization */
  assert(nsd > 0);
  assert(nsd <= 16);

  /* How many bits to preserve? */
  prc_bnr_xct=nsd*bit_per_dcm_dgt_prc;
  /* Be conservative, round upwards */
  prc_bnr_ceil=(unsigned short)ceil(prc_bnr_xct);
  /* First bit is implicit not explicit but corner cases prevent our taking advantage of this */
  //prc_bnr_xpl_rqr=prc_bnr_ceil-1; /* 20201223 CSZ verified this fails for small integers with NSD=1 */
  //prc_bnr_xpl_rqr=prc_bnr_ceil;
  prc_bnr_xpl_rqr=prc_bnr_ceil+1;
  if(type == NC_DOUBLE) prc_bnr_xpl_rqr++; /* Seems necessary for double-precision ppc=array(1.234567,1.0e-6,$dmn) */

  if(type == NC_FLOAT  && prc_bnr_xpl_rqr >= bit_xpl_nbr_sgn_flt) return;
  if(type == NC_DOUBLE && prc_bnr_xpl_rqr >= bit_xpl_nbr_sgn_dbl) return;

  switch(type){
  case NC_FLOAT:
    bit_xpl_nbr_sgn=bit_xpl_nbr_sgn_flt;
    bit_xpl_nbr_zro=bit_xpl_nbr_sgn-prc_bnr_xpl_rqr;
    assert(bit_xpl_nbr_zro <= bit_xpl_nbr_sgn-NCO_PPC_BIT_XPL_NBR_MIN);
    u32_ptr=op1.ui32p;
    /* Create mask */
    msk_f32_u32_zro=0u; /* Zero all bits */
    msk_f32_u32_zro=~msk_f32_u32_zro; /* Turn all bits to ones */
    /* Bit Shave mask for AND: Left shift zeros into bits to be rounded, leave ones in untouched bits */
    msk_f32_u32_zro <<= bit_xpl_nbr_zro;
    /* Bit Set   mask for OR:  Put ones into bits to be set, zeros in untouched bits */
    msk_f32_u32_one=~msk_f32_u32_zro;
    //msk_f32_u32_hshv=msk_f32_u32_one & (msk_f32_u32_zro >> 1); /* Set one bit: the MSB of LSBs */

    /* Bit-Groom: alternately shave and set LSBs */
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx+=2L) u32_ptr[idx]&=msk_f32_u32_zro;
      for(idx=1L;idx<sz;idx+=2L)
	if(u32_ptr[idx] != 0U) /* Never quantize upwards floating point values of zero */
	  u32_ptr[idx]|=msk_f32_u32_one;
    }else{ /* !has_mss_val */
      const float mss_val_flt=*mss_val.fp;
      for(idx=0L;idx<sz;idx+=2L)
	if(op1.fp[idx] != mss_val_flt) u32_ptr[idx]&=msk_f32_u32_zro;
      for(idx=1L;idx<sz;idx+=2L)
	if(op1.fp[idx] != mss_val_flt && u32_ptr[idx] != 0U) u32_ptr[idx]|=msk_f32_u32_one;
    } /* !has_mss_val */
    break; /* !NC_FLOAT */
  case NC_DOUBLE:
    bit_xpl_nbr_sgn=bit_xpl_nbr_sgn_dbl;
    bit_xpl_nbr_zro=bit_xpl_nbr_sgn-prc_bnr_xpl_rqr;
    assert(bit_xpl_nbr_zro <= bit_xpl_nbr_sgn-NCO_PPC_BIT_XPL_NBR_MIN);
    u64_ptr=(unsigned long long int *)op1.ui64p;
    /* Create mask */
    msk_f64_u64_zro=0ul; /* Zero all bits */
    msk_f64_u64_zro=~msk_f64_u64_zro; /* Turn all bits to ones */
    /* Bit Shave mask for AND: Left shift zeros into bits to be rounded, leave ones in untouched bits */
    msk_f64_u64_zro <<= bit_xpl_nbr_zro;
    /* Bit Set   mask for OR:  Put ones into bits to be set, zeros in untouched bits */
    msk_f64_u64_one=~msk_f64_u64_zro;
    //msk_f64_u64_hshv=msk_f64_u64_one & (msk_f64_u64_zro >> 1); /* Set one bit: the MSB of LSBs */
    /* Bit-Groom: alternately shave and set LSBs */
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx+=2L) u64_ptr[idx]&=msk_f64_u64_zro;
      for(idx=1L;idx<sz;idx+=2L)
	if(u64_ptr[idx] != 0UL) /* Never quantize upwards floating point values of zero */
	  u64_ptr[idx]|=msk_f64_u64_one;
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0L;idx<sz;idx+=2L)
	if(op1.dp[idx] != mss_val_dbl) u64_ptr[idx]&=msk_f64_u64_zro;
      for(idx=1L;idx<sz;idx+=2L)
	if(op1.dp[idx] != mss_val_dbl && u64_ptr[idx] != 0UL) u64_ptr[idx]|=msk_f64_u64_one;
    } /* end else */
    break; /* !NC_DOUBLE */
  default: 
    (void)fprintf(stderr,"ERROR: %s reports datum size = %d B is invalid for %s filter\n",fnc_nm,type,CCR_FLT_NAME);
    break;
  } /* !type */
  
} /* ccr_bgr() */
