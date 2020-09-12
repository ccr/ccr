/* 
   Copyright (C) 2020--present Charlie Zender

   Portions (can_apply(), set_local()) Copyright (c) 2019, CNES, have this license:
   This source code is licensed under MIT-style license (found in the
   COPYING file in the root directory of this source tree). */

 /*
 * This file is an example of an HDF5 filter plugin.
 * The plugin can be used with the HDF5 library vesrion 1.8.11+ to read and write
 * HDF5 datasets quantized with BitGrooming.
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

/* Standard header files */
// fxm: needed for M_LN10, M_LN2 in ccr_bgr() 
#ifdef HAVE_MATH_H
# include <math.h> /* sin cos cos sin 3.14159 */
#endif

/* 3rd party vendors */
#include "H5PLextern.h" /* HDF5 Plugin Library: H5PLget_plugin_type(), H5PLget_plugin_info() */

/* Tokens and typedefs */
#define H5Z_FILTER_BITGROOM 37373 /* fxm: Placeholder Filter ID during development */
#define CCR_FLT_NAME "HDF5 BitGroom filter"
#define CCR_FLT_PRM_NBR 3 /* [nbr] Number of parameters sent to filter (in cd_params array) */
#define CCR_FLT_PRM_PSN_NSD 0 /* [nbr] Ordinal position of NSD in parameter list (cd_params array) */
#define CCR_FLT_PRM_PSN_DATUM_SIZE 1 /* [nbr] Ordinal position of datum_size in parameter list (cd_params array) */
#define CCR_FLT_PRM_PSN_DATA_CLASS 2 /* [nbr] Ordinal position of data_class in parameter list (cd_params array) */
#define CCR_FLT_BGR_NSD_DFL 3 /* [nbr] Default number of significant digits for quantization */

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
size_t /* O [B] Number of bytes processed from input buffer (?) */
H5Z_filter_bitgroom /* [fnc] HDF5 BitGroom Filter */
(unsigned int flags, /* I [flg] Bitfield that encodes filter direction */
 size_t cd_nelmts, /* I [nbr] Number of elements in filter parameter (cd_values[]) array */
 const unsigned int cd_values[], /* I [enm] Filter parameters */
 size_t nbytes, /* I [B] Number of bytes in input buffer (before quantization/compression) */
 size_t *buf_size, /* O [B] Number of bytes in output buffer (after quantization/compression) */
 void **bfr_inout); /* I/O [frc] Values to quantize */

htri_t /* O [flg] Data meet criteria to apply filter */
can_apply /* [fnc] Callback to determine if current variable meets filter criteria */
(hid_t dcpl, /* I [id] Property list ID */
 hid_t type, /* I [id] Dataset type ID */
 hid_t space); /* I [id] Dataset space ID */

htri_t /* O [flg] Filter parameters successfully modified for this variable */
set_local /* [fnc] Callback to determine and set per-variable filter parameters */
(hid_t dcpl, /* I [id] Property list ID */
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
    can_apply, /* [fnc] Callback to determine if current variable meets filter criteria */
    set_local, /* [fnc] Callback to determine and set per-variable filter parameters */
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

/* Forward declaration for eventual inclusion of DigitRounding quantization */
/*
 * Round the float value keeping nsd significant digits. Fast computation method.
 *
 * The fast method uses an approximation of the number of digit before the floating point
 * instead of using log10() function.
 *
 */
double ccr_dgr(double v, int nsd);

/* Function definitions */
H5PL_type_t /* O [enm] Plugin type */
H5PLget_plugin_type /* [fnc] Provide plug-in type provided by this shared library */
(void)
{ /* Purpose: Provide plug-in type provided by this shared library to the HDF5 filter plugin mechanism
     This function is usually called after an application calls H5Pset_filter(), or when the data to which this filter will be applied are first read */
  return H5PL_TYPE_FILTER;
} /* !H5PLget_plugin_type() */

const void * /* O [enm] */
H5PLget_plugin_info /* [fnc] Return structure */
(void)
{ /* Purpose: Provide structure that defines BitGroom filter so the filter may be dynamically registered with the plugin mechanism
     This function is usually called after an application calls H5Pset_filter(), or when the data to which this filter will be applied are first read */
  return H5Z_BITGROOM;
} /* !H5PLget_plugin_info() */

size_t /* O [B] Number of input buffer bytes processed by this invocation of filter (?) */
H5Z_filter_bitgroom /* [fnc] HDF5 BitGroom Filter */
(unsigned int flags, /* I [flg] Bitfield that encodes filter direction */
 size_t cd_nelmts, /* I [nbr] Number of elements in filter parameter (cd_values[]) array */
 const unsigned int cd_values[], /* I [enm] Filter parameters */
 size_t nbytes, /* I [B] Number of bytes in input buffer (before quantization/compression) */
 size_t *buf_size, /* O [B] Number of bytes in output buffer (after quantization/compression) */
 void **bfr_inout) /* I/O [frc] Values to quantize */
{
  /* Purpose: Dynamic filter invoked by HDF5 to BitGroom a variable */

  const char fnc_nm[]="H5Z_filter_bitgroom()"; /* [sng] Function name */

  if(flags & H5Z_FLAG_REVERSE){

    /* Currently supported quantization methods (BitGrooming, fxm) store results in IEEE754 format 
       These quantized buffers are full of legal IEEE754 and need no decompression */
    return nbytes;

  }else{ /* !flags */

    /* Set parameters needed by quantization library filter */
    int nsd=cd_values[CCR_FLT_PRM_PSN_NSD];
    size_t datum_size=cd_values[CCR_FLT_PRM_PSN_DATUM_SIZE];
    H5T_class_t data_class=(H5T_class_t)cd_values[CCR_FLT_PRM_PSN_DATA_CLASS];
    // 20200912: fxm account for missing values
    int has_mss_val=0; /* [flg] Flag for missing values */
    //int has_mss_val=cd_values[CCR_FLT_PRM_PSN_HAS_MSS_VAL]; /* [flg] Flag for missing values */
    ptr_unn mss_val=(ptr_unn)NULL; /* [val] Value of missing value */
    //ptr_unn mss_val=cd_values[CCR_FLT_PRM_PSN_MSS_VAL]; /* [val] Value of missing value */
    ptr_unn op1; /* I/O [frc] Values to quantize */
    
    /* Quantization is only for floating-point data */
    if(data_class == H5T_FLOAT){ 
      switch(datum_size){
	/* Cast input buffer pointer to correct numeric type */
      case 4:
	/* Single-precision floating-point data */
	op1.fp=(float *)(*bfr_inout);
	ccr_bgr(nsd,NC_FLOAT,nbytes/sizeof(float),has_mss_val,mss_val,op1);
	break;
      case 8:
	/* Double-precision floating-point data */
	op1.dp=(double *)(*bfr_inout);
	ccr_bgr(nsd,NC_DOUBLE,nbytes/sizeof(double),has_mss_val,mss_val,op1);
	break;
      default:
	(void)fprintf(stderr, "ERROR: %s reports datum size = %lu B is invalid for %s filter\n",fnc_nm,datum_size,CCR_FLT_NAME);
	goto error;
	break;
      } /* !datum_size */
    }else{
      (void)fprintf(stderr, "ERROR: %s reports data type class identifier = %d is invalid for %s filter\n",fnc_nm,data_class,CCR_FLT_NAME);
      goto error;
    } /* !data_class */

    return nbytes;

  } /* !flags */
  
  return 1;

 error:
  /* Quantization filters generally allocate no memory, so just return with error code */
  return 0;

} /* !H5Z_filter_bitgroom() */

htri_t /* O [flg] Data meet criteria to apply filter */
can_apply /* [fnc] Callback to determine if current variable meets filter criteria */
(hid_t dcpl, /* I [id] Property list ID */
 hid_t type, /* I [id] Dataset type ID */
 hid_t space) /* I [id] Dataset space ID */
{
  /* Data space must be simple, i.e., a multi-dimensional array */
  if(H5Sis_simple(space) <= 0){
    fprintf(stderr,"Warning: Cannot apply %s filter. Data space is not simple.\n",CCR_FLT_NAME);
    return 0;
  } /* !H5Sis_simple(space) */

  /* Filter can be applied */
  return 1;
} /* !can_apply() */

htri_t /* O [flg] Filter parameters successfully modified for this variable */
set_local /* [fnc] Callback to determine and set per-variable filter parameters */
(hid_t dcpl, /* I [id] Property list ID */
 hid_t type, /* I [id] Dataset type ID */
 hid_t space) /* I [id] Dataset space ID */
{
  const char fnc_nm[]="set_local()"; /* [sng] Function name */

  herr_t rcd; /* [flg] Return code */
  
  /* Initialize filter parameters with default values */
  unsigned int ccr_flt_prm[CCR_FLT_PRM_NBR]={CCR_FLT_BGR_NSD_DFL,0,0};

  /* Initialize output variables for call to H5Pget_filter_by_id() */
  unsigned int flags=0;
  size_t cd_nelmts=CCR_FLT_PRM_NBR;
  unsigned int *cd_values=ccr_flt_prm;

  /* Retrieve parameters specified by user
     https://support.hdfgroup.org/HDF5/doc/RM/RM_H5P.html#FunctionIndex
     Ignore name and filter_config by setting last three arguments to 0/NULL */
  rcd=H5Pget_filter_by_id(dcpl,H5Z_FILTER_BITGROOM,&flags,&cd_nelmts,cd_values,0,NULL,NULL);
  if(rcd < 0){
    (void)fprintf(stderr,"ERROR: %s reports H5Pget_filter_by_id() failed to get %s filter flags and parameters for current variable\n",fnc_nm,CCR_FLT_NAME);
    return 0;
  } /* !rcd */

  /* Data type for this variable */
  int data_class; /* [enm] Data type class identifier (HDF5 equivalent of nc_type) */
  data_class=(int)H5Tget_class(type); 
  if(data_class < 0){
    (void)fprintf(stderr, "ERROR: %s reports H5Tget_class() returned invalid data type class identifier = %d for configuring %s filter for current variable\n",fnc_nm,data_class,CCR_FLT_NAME);
    return 0;
  } /* !data_class */
  /* Set data class in filter parameter list */
  ccr_flt_prm[CCR_FLT_PRM_PSN_DATA_CLASS]=data_class;

  /* Datum size for this variable */
  size_t datum_size; /* [B] Bytes per data value */
  datum_size=H5Tget_size(type);
  if(datum_size <= 0){
    (void)fprintf(stderr,"ERROR: %s reports H5Tget_size() returned invalid datum size = %lu B for %s filter\n",fnc_nm,datum_size,CCR_FLT_NAME);
    return 0;
  } /* !datum_size */
  /* Set datum size in filter parameter list */
  ccr_flt_prm[CCR_FLT_PRM_PSN_DATUM_SIZE]=datum_size;

  /* 20200911 fxm find, set, and pass per-variable hss_mss_val and mss_val arguments here
     attr=H5ACreate(dataset,"_FillValue",H5T_NATIVE_FLOAT,H5SCreate(H5S_SCALAR),H5P_DEFAULT);
     H5AWrite(attr,H5T_NATIVE_FLOAT,&value); */

  /* Update invoked filter with generic parameters as invoked with variable-specific values */
  rcd=H5Pmodify_filter(dcpl,H5Z_FILTER_BITGROOM,flags,CCR_FLT_PRM_NBR,cd_values);
  if(rcd < 0){
    (void)fprintf(stderr,"ERROR: %s reports H5Pmodify_filter() unable to set %s filter parameters\n",fnc_nm,CCR_FLT_NAME);
    return 0;
  } /* !rcd */

  return 1;
} /* !set_local() */

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

  /* Use constants defined in math.h */
  const double bit_per_dcm_dgt_prc=M_LN10/M_LN2; /* 3.32 [frc] Bits per decimal digit of precision */
  const double dcm_per_bit_dgt_prc=M_LN2/M_LN10; /* 0.301 [frc] Bits per decimal digit of precision */
  
  const int bit_xpl_nbr_sgn_flt=23; /* [nbr] Bits 0-22 of SP significands are explicit. Bit 23 is implicitly 1. */
  const int bit_xpl_nbr_sgn_dbl=53; /* [nbr] Bits 0-52 of DP significands are explicit. Bit 53 is implicitly 1. */
  const int ieee_xpn_fst_flt=127; /* [nbr] IEEE "exponent bias" = actual exponent minus stored exponent */
  
  double prc_bnr_xct; /* [nbr] Binary digits of precision, exact */
  
  int bit_xpl_nbr_sgn=-1; /* [nbr] Number of explicit bits in significand */
  int bit_xpl_nbr_zro; /* [nbr] Number of explicit bits to zero */

  size_t idx;

  unsigned int *u32_ptr;
  unsigned int msk_f32_u32_zro;
  unsigned int msk_f32_u32_one;
  unsigned int msk_f32_u32_hshv;
  unsigned long int *u64_ptr;
  unsigned long int msk_f64_u64_zro;
  unsigned long int msk_f64_u64_one;
  unsigned long int msk_f64_u64_hshv;
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
  //prc_bnr_xpl_rqr=prc_bnr_ceil-1;
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
    msk_f32_u32_hshv=msk_f32_u32_one & (msk_f32_u32_zro >> 1); /* Set one bit: the MSB of LSBs */

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
    u64_ptr=(unsigned long int *)op1.ui64p;
    /* Create mask */
    msk_f64_u64_zro=0ul; /* Zero all bits */
    msk_f64_u64_zro=~msk_f64_u64_zro; /* Turn all bits to ones */
    /* Bit Shave mask for AND: Left shift zeros into bits to be rounded, leave ones in untouched bits */
    msk_f64_u64_zro <<= bit_xpl_nbr_zro;
    /* Bit Set   mask for OR:  Put ones into bits to be set, zeros in untouched bits */
    msk_f64_u64_one=~msk_f64_u64_zro;
    msk_f64_u64_hshv=msk_f64_u64_one & (msk_f64_u64_zro >> 1); /* Set one bit: the MSB of LSBs */
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
    (void)fprintf(stderr, "ERROR: %s reports datum size = %d B is invalid for %s filter\n",fnc_nm,type,CCR_FLT_NAME);
    break;
  } /* !type */
  
} /* ccr_bgr() */
  
void
ccr_dgr_flt /* [fnc] DigitRound buffer of float values */
(const int nsd, /* I [nbr] Number of decimal significant digits to quantize to */
 const size_t nbytes, /* I [B] Buffer size */
 void **bfr_inout) /* I/O [frc] Values to quantize */
{
  /* Cast input buffer to float */
  float *bfr_flt=(float *)*bfr_inout;

  /* DigitRound array */
  for(size_t idx=0;idx<nbytes/sizeof(float);idx++)
    bfr_flt[idx]=(float)ccr_dgr((double)bfr_flt[idx],nsd);
} /* ccr_dgr_flt() */

void
ccr_dgr_dbl /* [fnc] DigitRound buffer of double values */
(const int nsd, /* I [nbr] Number of decimal significant digits to quantize to */
 const size_t nbytes, /* I [B] Buffer size */
 const void **bfr_inout) /* I/O [frc] Values to quantize */
{
  /* Cast input buffer to double */
  double *bfr_dbl=(double *)*bfr_inout;

  /* DigitRound array */
  for(size_t idx=0;idx < nbytes/sizeof(double);idx++)
    bfr_dbl[idx]=ccr_dgr(bfr_dbl[idx],nsd);
} /* ccr_dgr_dbl() */
