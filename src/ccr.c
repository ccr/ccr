/**
 * @file
 * This is the code file for the Community Codex Repository library
 * for netCDF.
 *
 * @author Ed Hartnett
 * @date 12/30/19
 */

/**
 * @mainpage CCR
 *
 * The Community Codec Repository supports compression (and other)
 * filters for netCDF/HDF5 files which are not natively supported by
 * the netCDF C library.
 *
 * BZIP2
 *
 * Bzip2 is a free and open-source file compression program that uses
 * the Burrows–Wheeler algorithm. For more information see
 * https://www.sourceware.org/bzip2/ and
 * https://en.wikipedia.org/wiki/Bzip2.
 *
 * In C:
 * - nc_def_var_bzip2()
 * - nc_inq_var_bzip2()
 *
 * In Fortran:
 * - nf90_def_var_bzip2()
 * - nf90_inq_var_bzip2()
 *
 * BitGroom
 *
 * The BitGroom filter quantizes the mantissa of floating point values
 * (integers are unaffected) by the appropriate amount to retain not
 * less than the requested Number of Significant Digits (NSD), usually
 * taken as the intrinsic precision of the measured or modeled data.
 * BitGroomed data remain in IEEE-754 format, and are more accurate
 * than other quantization filters such as BitShaving and BitSetting.
 * Consider BitGroom as a pre-filter for subsequent lossless compression 
 * which with the simple mantissas yields better compression ratios.
 * Zender, C. S. (2016), Bit Grooming: Statistically accurate 
 * precision-preserving quantization with compression, evaluated in 
 * the netCDF Operators (NCO, v4.4.8+), Geosci. Model Dev., 9, 
 * 3199-3211, doi:10.5194/gmd-9-3199-2016.
 * http://www.geosci-model-dev.net/9/3199/2016
 * For more info see http://nco.sf.net/nco.html#bg.
 *
 * In C:
 * - nc_def_var_bitgroom()
 * - nc_inq_var_bitgroom()
 *
 * In Fortran:
 * - nf90_def_var_bitgroom()
 * - nf90_inq_var_bitgroom()
 *
 * Granular BitGroom
 *
 * The Granular BitGroom (GBG) filter quantizes the mantissa of floating
 * point values (integers are unaffected) by the appropriate amount to
 * retain not less than the requested Number of Significant Digits (NSD),
 * usually taken as the intrinsic precision of the measured or modeled data.
 * GBG is a second generation quantization filter that incorporates aspects
 * of the BitGroom, DigitRound, and BitRound algorithms.
 * GBG determines the quantization mask for each value independently using
 * the basse-10 logarithm method discussed in the DigitRound paper. 
 * GBG computes exact log10() of every number, in contrast to DigitRound
 * which uses a lookup table for speed. This makes GBG slower than DR.
 * However, GBG-quantized values will use the same mask regardless of the
 * sign of the input value, where DR might produce slightly different masks.
 * Another difference is that GBG uses the BitRound rounding method, rather
 * than reconstructing the scalar quantization at bin center.
 * Similar to BitGroom, GBG ignores the value zero, and the _FillValue.
 * GBG quantizes a few more bits than BitGroom from the raw number yet
 * still maintains the precision guarantee, thus the quantization error 
 * of GBG is larger than BitGroom. GBG improver compression rations 
 * by ~20% relative to BitGroom for typical climate data with NSD = 3.
 * Consider GBG as a pre-filter for subsequent lossless compression 
 * which with the simple mantissas yields better compression ratios.
 * For more info see http://nco.sf.net/nco.html#gbg.
 *
 * In C:
 * - nc_def_var_granularbg()
 * - nc_inq_var_granularbg()
 *
 * In Fortran:
 * - nf90_def_var_granularbg()
 * - nf90_inq_var_granularbg()
 *
 * Zstandard
 *
 * From the Zstandard documentation: "Zstandard is a fast compression
 * algorithm, providing high compression ratios. It also offers a
 * special mode for small data, called dictionary compression. The
 * reference library offers a very wide range of speed / compression
 * trade-off, and is backed by an extremely fast decoder (see
 * benchmarks below). Zstandard library is provided as open source
 * software using a BSD license. Its format is stable and published as
 * IETF RFC 8478." For more info see https://facebook.github.io/zstd/.
 *
 * In C:
 * - nc_def_var_zstandard()
 * - nc_inq_var_zstandard()
 *
 * In Fortran:
 * - nf90_def_var_zstandard()
 * - nf90_inq_var_zstandard()
 *
 * @image html NetCDF_Filters.png
 *
 */

#include "config.h"
#include "ccr.h"
#include <hdf5.h>
#include <H5DSpublic.h>
#include <stdlib.h>

#define MAX_BITGROOM_NSD_FLOAT 7
#define MAX_BITGROOM_NSD_DOUBLE 15
#define MAX_GRANULARBG_NSD_FLOAT 7
#define MAX_GRANULARBG_NSD_DOUBLE 15

/**
 * Turn on bzip2 compression for a variable.
 *
 * @param ncid File ID.
 * @param varid Variable ID.
 * @param level From 1 to 9. Set the block size to 100k, 200k ... 900k
 * when compressing. (bzip2 default level is 9).
 *
 * @return 0 for success, error code otherwise.
 * @author Ed Hartnett
 */
int
nc_def_var_bzip2(int ncid, int varid, int level)
{
    unsigned int cd_value = level;
    int ret;

    /* Level must be between 1 and 9. */
    if (level < 1 || level > 9)
        return NC_EINVAL;

    if (!H5Zfilter_avail(BZIP2_ID))
    {
        printf ("bzip2 filter not available.\n");
        return NC_EFILTER;
    }
    /* Set up the bzip2 filter for this var. */
    if ((ret = nc_def_var_filter(ncid, varid, BZIP2_ID, 1, &cd_value)))
        return ret;

    return 0;
}

/**
 * Learn whether bzip2 compression is on for a variable, and, if so,
 * the level setting.
 *
 * @param ncid File ID.
 * @param varid Variable ID.
 * @param bzip2p Pointer that gets a 0 if bzip2 is not in use for this
 * var, and a 1 if it is. Ignored if NULL.
 * @param levelp Pointer that gets the level setting (from 1 to 9), if
 * bzip2 is in use. Ignored if NULL.
 *
 * @return 0 for success, error code otherwise.
 * @author Ed Hartnett
 */
int
nc_inq_var_bzip2(int ncid, int varid, int *bzip2p, int *levelp)
{
    unsigned int level;
    size_t nparams;
    int bzip2 = 0; /* Is bzip2 in use? */
    int ret;

#ifdef HAVE_MULTIFILTERS
    {
	size_t nfilters;
	unsigned int *filterids;
	int f;
	
	/* Get filter information. */
	if ((ret = nc_inq_var_filter_ids(ncid, varid, &nfilters, NULL)))
	    return ret;
	
	/* If there are no filters, we're done. */
	if (nfilters == 0)
	{
	    if (bzip2p)
		*bzip2p = 0;
	    return 0;
	}

	/* Allocate storage for filter IDs. */
	if (!(filterids = malloc(nfilters * sizeof(unsigned int))))
	    return NC_ENOMEM;

	/* Get the filter IDs. */
	if ((ret = nc_inq_var_filter_ids(ncid, varid, &nfilters, filterids)))
	    return ret;
    
	/* Check each filter to see if it is Bzip2. */
	for (f = 0; f < nfilters; f++)
	{
	    if (filterids[f] == BZIP2_ID)
		bzip2++;

	    /* If Bzip2 is in use, check parameter. */
	    if (bzip2)
	    {
	    
		if ((ret = nc_inq_var_filter_info(ncid, varid, filterids[f], &nparams, &level)))
		    return ret;

		/* For Bzip2, there is one parameter. */
		if (nparams != 1)
		    return NC_EFILTER;

		/* Tell the caller, if they want to know. */
		if (levelp)
		    *levelp = (int)level;

		/* Exit loop to report parameters (neglect remaining filters) */
		break;
	    }
	}

	/* Free resources. */
	free(filterids);

	if (bzip2p)
	    *bzip2p = bzip2;
    }
#else
    {
	unsigned int id;
	
	/* Get filter information. */
	ret = nc_inq_var_filter(ncid, varid, &id, &nparams, &level);
	if (ret == NC_ENOFILTER)
	{
	    /* No filter means no bzip2. */
	    if (bzip2p)
		*bzip2p = 0;
	    return 0;
	}
	else if (ret)
	    return ret;

	/* Is bzip2 in use? */
	if (id == BZIP2_ID)
	    bzip2++;

	/* Does caller want to know if bzip2 is in use? */
	if (bzip2p)
	    *bzip2p = bzip2;

	/* If bzip2 is in use, check parameter. */
	if (bzip2)
	{
	    /* For bzip2, there is one parameter. */
	    if (nparams != 1)
		return NC_EFILTER;

	    /* Tell the caller, if they want to know. */
	    if (levelp)
		*levelp = level;
	}
    }
#endif

    return 0;
}

/* /\** */
/*  * Turn on lz4 compression for a variable. */
/*  * */
/*  * @param ncid File ID. */
/*  * @param varid Variable ID. */
/*  * @param level From 1 to 9. Set the block size to 100k, 200k ... 900k */
/*  * when compressing. (lz4 default level is 9). */
/*  * */
/*  * @return 0 for success, error code otherwise. */
/*  * @author Ed Hartnett */
/*  *\/ */
/* int */
/* nc_def_var_lz4(int ncid, int varid, int level) */
/* { */
/*     unsigned int cd_value = level; */
/*     int ret; */

/*     /\* Level must be between 1 and 9. *\/ */
/*     if (level < 1 || level > 9) */
/*         return NC_EINVAL; */

/*     if (!H5Zfilter_avail(LZ4_ID)) */
/*     { */
/*         printf ("lz4 filter not available.\n"); */
/*         return NC_EFILTER; */
/*     } */

/*     /\* Set up the lz4 filter for this var. *\/ */
/*     if ((ret = nc_def_var_filter(ncid, varid, LZ4_ID, 1, &cd_value))) */
/*         return ret; */

/*     return 0; */
/* } */

/* /\** */
/*  * Learn whether lz4 compression is on for a variable, and, if so, */
/*  * the level setting. */
/*  * */
/*  * @param ncid File ID. */
/*  * @param varid Variable ID. */
/*  * @param lz4p Pointer that gets a 0 if lz4 is not in use for this */
/*  * var, and a 1 if it is. Ignored if NULL. */
/*  * @param levelp Pointer that gets the level setting (from 1 to 9), if */
/*  * bzlip2 is in use. Ignored if NULL. */
/*  * */
/*  * @return 0 for success, error code otherwise. */
/*  * @author Ed Hartnett */
/*  *\/ */
/* int */
/* nc_inq_var_lz4(int ncid, int varid, int *lz4p, int *levelp) */
/* { */
/*     unsigned int level; */
/*     unsigned int id; */
/*     size_t nparams; */
/*     int lz4 = 0; /\* Is lz4 in use? *\/ */
/*     int ret; */

/*     /\* Get filter information. *\/ */
/*     ret = nc_inq_var_filter(ncid, varid, &id, &nparams, &level); */
/*     if (ret == NC_ENOFILTER) */
/*     { */
/* 	if (lz4p) */
/* 	    *lz4p = 0; */
/* 	return 0; */
/*     } */
/*     else if (ret) */
/* 	return ret; */

/*     /\* Is lz4 in use? *\/ */
/*     if (id == LZ4_ID) */
/*         lz4++; */

/*     /\* Does caller want to know if lz4 is in use? *\/ */
/*     if (lz4p) */
/*         *lz4p = lz4; */

/*     /\* If lz4 is in use, check parameter. *\/ */
/*     if (lz4) */
/*     { */
/*         /\* For lz4, there is one parameter. *\/ */
/*         if (nparams != 1) */
/*             return NC_EFILTER; */

/*         /\* Tell the caller, if they want to know. *\/ */
/*         if (levelp) */
/*             *levelp = level; */
/*     } */

/*     return 0; */
/* } */

/**
 * Turn on BitGroom quantization for a variable.
 *
 * The BitGroom filter quantizes the data by setting unneeded bits
 * alternately to 1/0, so that they may compress well. The BitGroom
 * filter itself is lossy (data are irretrievably altered), and it
 * improves the compression ratio provided by a subsequent lossless 
 * compression filter. Call the nc_def_var_bitgroom() function before
 * the function that turns on the lossless compression filter
 * (nc_def_var_deflate(), for example). 
 *
 * A notable feature of BitGroom is that the data it processes remain 
 * in IEEE754 format after quantization. Therefore the BitGroom filter 
 * does nothing when data are read. However, the Bitgroom filter must 
 * still be installed on machines that need to read bitgroomed data.
 *
 * The BitGroom filter only quantizes variables of type NC_FLOAT or
 * NC_DOUBLE. Attempts to set the BitGroom filter for other variable
 * types through the C/Fortran API return an error (NC_EINVAL). The
 * filter does not quantize values equal to the value of the
 * _FillValue attribute, if any. The main difference between the
 * BitGroom algorithm as implemented in the CCR and in NCO is that the
 * NCO version will not quantize the values of "coordinate-like"
 * variables (e.g., latitude, longitude, time) as defined in the NCO
 * manual, whereas the CCR version will quantize any floating-point
 * variable.
 *
 * @note Internally, the filter requires CCR_FLT_PRM_NBR (=5) elements
 * for cd_value. However, the user needs to provide only the first
 * element, NSD, since the other elements can be and are derived from
 * the dcpl (data_class, datum_size), and extra queries of the
 * variable (has_mss_val, mss_val). Hence, the netCDF API exposes to
 * the user and requires setting only the minimal number (1) of filter
 * parameters.

 * @param ncid File ID.
 * @param varid Variable ID.
 * @param nsd Number of significant digits to retain. Allowed single- and
 * double-precision NSDs are 1-7 and 1-15, respectively. (Default is 3).
 *
 * @return 0 for success, error code otherwise.
 * @author Charlie Zender
 */
int
nc_def_var_bitgroom(int ncid, int varid, int nsd)
{
  unsigned int cd_value[BITGROOM_FLT_PRM_NBR];
  int ret;
  nc_type var_typ;
  
  /* BitGroom only quantizes floating-point values */
  if ((ret = nc_inq_vartype(ncid, varid, &var_typ)))
    return ret;

  if (var_typ != NC_FLOAT && var_typ != NC_DOUBLE)
  {
      /* printf ("BitGroom filter can only be defined for floating-point variables.\n"); */
      return NC_EINVAL;
  }
  
  /* NSD must be between 1 and 7 for NC_FLOAT, 1 and 15 for
   * NC_DOUBLE. */
  if (nsd < 1 || nsd > (var_typ == NC_FLOAT ? MAX_BITGROOM_NSD_FLOAT : MAX_BITGROOM_NSD_DOUBLE))
    return NC_EINVAL;

  if (!H5Zfilter_avail(BITGROOM_ID))
  {
      printf ("BitGroom filter not available.\n");
      return NC_EFILTER;
  }

  /* User-provided NSD is first element of filter parameter array */
  cd_value[0] = nsd;

  /* Set up the BitGroom filter for this var. */
  if ((ret = nc_def_var_filter(ncid, varid, BITGROOM_ID, BITGROOM_FLT_PRM_NBR, cd_value)))
    return ret;

  return 0;
}

/**
 * Learn whether BitGroom quantization is on for a variable, and, if so,
 * the NSD setting.
 *
 * @param ncid File ID.
 * @param varid Variable ID.
 * @param bitgroomp Pointer that gets a 0 if BitGroom is not in use for this
 * var, and a 1 if it is. Ignored if NULL.
 * @param nsdp Pointer that gets the NSD setting (from 1 to 15), if
 * BitGroom is in use. Ignored if NULL.
 *
 * @return 0 for success, error code otherwise.
 * @author Charlie Zender
 */
int
nc_inq_var_bitgroom(int ncid, int varid, int *bitgroomp, int *nsdp)
{
  unsigned int nsd[BITGROOM_FLT_PRM_NBR];
  size_t nparams;
  int bitgroom = 0; /* Is BitGroom in use? */
  int ret;
  
#ifdef HAVE_MULTIFILTERS
    {
	size_t nfilters;
	unsigned int *filterids;
	int f;
	
	/* Get filter information. */
	if ((ret = nc_inq_var_filter_ids(ncid, varid, &nfilters, NULL)))
	    return ret;
	
	/* If there are no filters, we're done. */
	if (nfilters == 0)
	{
	    if (bitgroomp)
		*bitgroomp = 0;
	    return 0;
	}

	/* Allocate storage for filter IDs. */
	if (!(filterids = malloc(nfilters * sizeof(unsigned int))))
	    return NC_ENOMEM;

	/* Get the filter IDs. */
	if ((ret = nc_inq_var_filter_ids(ncid, varid, &nfilters, filterids)))
	    return ret;
    
	/* Check each filter to see if it is BitGroom. */
	for (f = 0; f < nfilters; f++)
	{
	    if (filterids[f] == BITGROOM_ID)
		bitgroom++;

	    /* If BitGroom is in use, check parameter. */
	    if (bitgroom)
	    {
	    
		if ((ret = nc_inq_var_filter_info(ncid, varid, filterids[f], &nparams, nsd)))
		    return ret;

		/* BitGroom has BITGROOM_FLT_PRM_NBR == 5 internal parameters.
		   We expose only the first (NSD) through this API because a variable's properties 
		   uniquely determine the remainder and exposing them to users, well, invites disaster */
		if (nparams != BITGROOM_FLT_PRM_NBR)
		  return NC_EFILTER;

		/* Tell the caller, if they want to know. */
		if (nsdp)
		    *nsdp = (int)nsd[0];

		/* Exit loop to report parameters (neglect remaining filters) */
		break;

	    }
	}

	/* Free resources. */
	free(filterids);

	/* Does caller want to know if BitGroom is in use? */
	if (bitgroomp)
	    *bitgroomp = bitgroom;
    }
#else
    {
	unsigned int id;

	/* Get filter information. */
	ret = nc_inq_var_filter(ncid, varid, &id, &nparams, nsd);
	if (ret == NC_ENOFILTER)
	  {
	    if (bitgroomp)
	      *bitgroomp = 0;
	    return 0;
	  }
	else if (ret)
	  return ret;
  
	/* Is BitGroom in use? */
	if (id == BITGROOM_ID)
	  bitgroom++;
  
	/* Does caller want to know if BitGroom is in use? */
	if (bitgroomp)
	  *bitgroomp = bitgroom;
  
	/* If BitGroom is in use, check parameter. */
	if (bitgroom)
	  {
	    /* BitGroom has BITGROOM_FLT_PRM_NBR == 5 internal parameters.
	       We expose only the first (NSD) through this API because a variable's properties 
	       uniquely determine the remainder and exposing them to users, well, invites disaster */
	    if (nparams != BITGROOM_FLT_PRM_NBR)
	      return NC_EFILTER;
      
	    /* Tell the caller, if they want to know. */
	    if (nsdp)
	      *nsdp =(int)nsd[0];
	  }
    }
#endif /* HAVE_MULTIFILTERS */
  return 0;
}

/**
 * Turn on Granular BitGroom quantization for a variable.
 *
 * The Granular BitGroom (GBG) filter quantizes the data by setting
 * unneeded bits to 0, so that they may compress well. GBG is in all
 * external respects similar to BitGroom. The following paragraph
 * describes the differences between GBG and BitGroom, and the rest
 * this preamble, and the postamble are identical to the BitGroom docs.
 * The filter itself is lossy (data are irretrievably altered), and it
 * improves the compression ratio provided by a subsequent lossless 
 * compression filter. Call the nc_def_var_granularbg() function before
 * the function that turns on the lossless compression filter
 * (nc_def_var_deflate(), for example). 
 *
 * Granular BitGroom is a second generation quantization filter that
 * incorporates aspects of the BitGroom, DigitRound, and BitRound algorithms.
 * GBG determines the quantization mask for each value independently using
 * the basse-10 logarithm method discussed in the DigitRound paper. 
 * GBG computes exact log10() of every number, in contrast to DigitRound
 * which uses a lookup table for speed. This makes GBG slower than DR.
 * However, GBG-quantized values will use the same mask regardless of the
 * sign of the input value, where DR might produce slightly different masks.
 * Another difference is that GBG uses the BitRound rounding method, rather
 * than reconstructing the scalar quantization at bin center.
 * Similar to BitGroom, GBG ignores the value zero, and the _FillValue.
 * GBG quantizes a few more bits than BitGroom from the raw number yet
 * still maintains the precision guarantee, thus the quantization error 
 * of GBG is larger than BitGroom. GBG improver compression rations 
 * by ~20% relative to BitGroom for typical climate data with NSD = 3.
 *
 * A notable feature of GranularBG is that the data it processes remain 
 * in IEEE754 format after quantization. Therefore the GranularBG filter 
 * does nothing when data are read. However, the Bitgroom filter must 
 * still be installed on machines that need to read bitgroomed data.
 *
 * The GranularBG filter only quantizes variables of type NC_FLOAT or
 * NC_DOUBLE. Attempts to set the GranularBG filter for other variable
 * types through the C/Fortran API return an error (NC_EINVAL). The
 * filter does not quantize values equal to the value of the
 * _FillValue attribute, if any. The main difference between the
 * GranularBG algorithm as implemented in the CCR and in NCO is that the
 * NCO version will not quantize the values of "coordinate-like"
 * variables (e.g., latitude, longitude, time) as defined in the NCO
 * manual, whereas the CCR version will quantize any floating-point
 * variable.
 *
 * @note Internally, the filter requires CCR_FLT_PRM_NBR (=5) elements
 * for cd_value. However, the user needs to provide only the first
 * element, NSD, since the other elements can be and are derived from
 * the dcpl (data_class, datum_size), and extra queries of the
 * variable (has_mss_val, mss_val). Hence, the netCDF API exposes to
 * the user and requires setting only the minimal number (1) of filter
 * parameters.

 * @param ncid File ID.
 * @param varid Variable ID.
 * @param nsd Number of significant digits to retain. Allowed single- and
 * double-precision NSDs are 1-7 and 1-15, respectively. (Default is 3).
 *
 * @return 0 for success, error code otherwise.
 * @author Charlie Zender
 */
int
nc_def_var_granularbg(int ncid, int varid, int nsd)
{
  unsigned int cd_value[GRANULARBG_FLT_PRM_NBR];
  int ret;
  nc_type var_typ;
  
  /* Granular BitGroom only quantizes floating-point values */
  if ((ret = nc_inq_vartype(ncid, varid, &var_typ)))
    return ret;

  if (var_typ != NC_FLOAT && var_typ != NC_DOUBLE)
  {
      /* printf ("Granular BitGroom filter can only be defined for floating-point variables.\n"); */
      return NC_EINVAL;
  }
  
  /* NSD must be between 1 and 7 for NC_FLOAT, 1 and 15 for
   * NC_DOUBLE. */
  if (nsd < 1 || nsd > (var_typ == NC_FLOAT ? MAX_GRANULARBG_NSD_FLOAT : MAX_GRANULARBG_NSD_DOUBLE))
    return NC_EINVAL;

  if (!H5Zfilter_avail(GRANULARBG_ID))
  {
      printf ("Granular BitGroom filter not available.\n");
      return NC_EFILTER;
  }

  /* User-provided NSD is first element of filter parameter array */
  cd_value[0] = nsd;

  /* Set up the Granular BitGroom filter for this var. */
  if ((ret = nc_def_var_filter(ncid, varid, GRANULARBG_ID, GRANULARBG_FLT_PRM_NBR, cd_value)))
    return ret;

  return 0;
}

/**
 * Learn whether Granular BitGroom quantization is on for a variable, and, if so,
 * the NSD setting.
 *
 * @param ncid File ID.
 * @param varid Variable ID.
 * @param granularbgp Pointer that gets a 0 if Granular BitGroom is not in use for this
 * var, and a 1 if it is. Ignored if NULL.
 * @param nsdp Pointer that gets the NSD setting (from 1 to 15), if
 * Granular BitGroom is in use. Ignored if NULL.
 *
 * @return 0 for success, error code otherwise.
 * @author Charlie Zender
 */
int
nc_inq_var_granularbg(int ncid, int varid, int *granularbgp, int *nsdp)
{
  unsigned int nsd[GRANULARBG_FLT_PRM_NBR];
  size_t nparams;
  int granularbg = 0; /* Is Granular BitGroom in use? */
  int ret;
  
#ifdef HAVE_MULTIFILTERS
    {
	size_t nfilters;
	unsigned int *filterids;
	int f;
	
	/* Get filter information. */
	if ((ret = nc_inq_var_filter_ids(ncid, varid, &nfilters, NULL)))
	    return ret;
	
	/* If there are no filters, we're done. */
	if (nfilters == 0)
	{
	    if (granularbgp)
		*granularbgp = 0;
	    return 0;
	}

	/* Allocate storage for filter IDs. */
	if (!(filterids = malloc(nfilters * sizeof(unsigned int))))
	    return NC_ENOMEM;

	/* Get the filter IDs. */
	if ((ret = nc_inq_var_filter_ids(ncid, varid, &nfilters, filterids)))
	    return ret;
    
	/* Check each filter to see if it is Granular BitGroom. */
	for (f = 0; f < nfilters; f++)
	{
	    if (filterids[f] == GRANULARBG_ID)
		granularbg++;

	    /* If Granular BitGroom is in use, check parameter. */
	    if (granularbg)
	    {
	    
		if ((ret = nc_inq_var_filter_info(ncid, varid, filterids[f], &nparams, nsd)))
		    return ret;

		/* Granular BitGroom has GRANULARBG_FLT_PRM_NBR == 5 internal parameters.
		   We expose only the first (NSD) through this API because a variable's properties 
		   uniquely determine the remainder and exposing them to users, well, invites disaster */
		if (nparams != GRANULARBG_FLT_PRM_NBR)
		  return NC_EFILTER;

		/* Tell the caller, if they want to know. */
		if (nsdp)
		    *nsdp = (int)nsd[0];

		/* Exit loop to report parameters (neglect remaining filters) */
		break;

	    }
	}

	/* Free resources. */
	free(filterids);

	/* Does caller want to know if Granular BitGroom is in use? */
	if (granularbgp)
	    *granularbgp = granularbg;
    }
#else
    {
	unsigned int id;

	/* Get filter information. */
	ret = nc_inq_var_filter(ncid, varid, &id, &nparams, nsd);
	if (ret == NC_ENOFILTER)
	  {
	    if (granularbgp)
	      *granularbgp = 0;
	    return 0;
	  }
	else if (ret)
	  return ret;
  
	/* Is Granular BitGroom in use? */
	if (id == GRANULARBG_ID)
	  granularbg++;
  
	/* Does caller want to know if Granular BitGroom is in use? */
	if (granularbgp)
	  *granularbgp = granularbg;
  
	/* If Granular BitGroom is in use, check parameter. */
	if (granularbg)
	  {
	    /* Granular BitGroom has GRANULARBG_FLT_PRM_NBR == 5 internal parameters.
	       We expose only the first (NSD) through this API because a variable's properties 
	       uniquely determine the remainder and exposing them to users, well, invites disaster */
	    if (nparams != GRANULARBG_FLT_PRM_NBR)
	      return NC_EFILTER;
      
	    /* Tell the caller, if they want to know. */
	    if (nsdp)
	      *nsdp =(int)nsd[0];
	  }
    }
#endif /* HAVE_MULTIFILTERS */
  return 0;
}

/**
 * Turn on Zstandard compression for a variable.
 *
 * @param ncid File ID.
 * @param varid Variable ID.
 * @param level From -131072 to 22 (depends on Zstandard version). 
 * when compressing. Regular compression levels are from 1 up to 19.
 * Use levels >= 20, labeled `--ultra`, cautiously: they require more memory. 
 * Negative compression levels that extend the range of speed vs. ratio preferences.
 * The lower the level, the faster the speed (at the cost of compression).
 *
 * @return 0 for success, error code otherwise.
 * @author Charlie Zender
 */
int
nc_def_var_zstandard(int ncid, int varid, int level)
{
    unsigned int cd_value = level;
    int ret;

    /* Level must be between -131072 and 22 on Zstandard v. 1.4.5 (~202009)
       Earlier versions have fewer levels (especially fewer negative levels) */
    if (level < -131072 || level > 22)
        return NC_EINVAL;

    if (!H5Zfilter_avail(ZSTANDARD_ID))
    {
        printf ("Zstandard filter not available.\n");
        return NC_EFILTER;
    }

    /* Set up the Zstandard filter for this var. */
    if ((ret = nc_def_var_filter(ncid, varid, ZSTANDARD_ID, 1, &cd_value)))
        return ret;

    return 0;
}

/**
 * Learn whether Zstandard compression is on for a variable, and, if so,
 * the level setting.
 *
 * @param ncid File ID.
 * @param varid Variable ID.
 * @param zstandardp Pointer that gets a 0 if Zstandard is not in use for this
 * var, and a 1 if it is. Ignored if NULL.
 * @param levelp Pointer that gets the level setting (from -131072 to 22), if
 * Zstandard is in use. Ignored if NULL.
 *
 * @return 0 for success, error code otherwise.
 * @author Charlie Zender
 */
int
nc_inq_var_zstandard(int ncid, int varid, int *zstandardp, int *levelp)
{
    int zstandard = 0; /* Is Zstandard in use? */
    unsigned int level;
    size_t nparams;
    int ret;

#ifdef HAVE_MULTIFILTERS
    {
	size_t nfilters;
	unsigned int *filterids;
	int f;
	
	/* Get filter information. */
	if ((ret = nc_inq_var_filter_ids(ncid, varid, &nfilters, NULL)))
	    return ret;
	
	/* If there are no filters, we're done. */
	if (nfilters == 0)
	{
	    if (zstandardp)
		*zstandardp = 0;
	    return 0;
	}

	/* Allocate storage for filter IDs. */
	if (!(filterids = malloc(nfilters * sizeof(unsigned int))))
	    return NC_ENOMEM;

	/* Get the filter IDs. */
	if ((ret = nc_inq_var_filter_ids(ncid, varid, &nfilters, filterids)))
	    return ret;
    
	/* Check each filter to see if it is Zstandard. */
	for (f = 0; f < nfilters; f++)
	{
	    if (filterids[f] == ZSTANDARD_ID)
		zstandard++;

	    /* If Zstandard is in use, check parameter. */
	    if (zstandard)
	    {
	    
		if ((ret = nc_inq_var_filter_info(ncid, varid, filterids[f], &nparams, &level)))
		    return ret;

		/* For Zstandard, there is one parameter. */
		if (nparams != 1)
		    return NC_EFILTER;

		/* Tell the caller, if they want to know. */
		if (levelp)
		    *levelp = (int)level;

		/* Exit loop to report parameters (neglect remaining filters) */
		break;

	    }
	}

	/* Free resources. */
	free(filterids);

	if (zstandardp)
	    *zstandardp = zstandard;
    }
#else
    {
	unsigned int id;
	
	/* Get filter information. */
	ret = nc_inq_var_filter(ncid, varid, &id, &nparams, &level);
	if (ret == NC_ENOFILTER)
	{
	    if (zstandardp)
		*zstandardp = 0;
	    return 0;
	}
	else if (ret)
	    return ret;
	
	/* Is Zstandard in use? */
	if (id == ZSTANDARD_ID)
	    zstandard++;
	
	/* Does caller want to know if Zstandard is in use? */
	if (zstandardp)
	    *zstandardp = zstandard;
	
	/* If Zstandard is in use, check parameter. */
	if (zstandard)
	{
	    /* For Zstandard, there is one parameter. */
	    if (nparams != 1)
		return NC_EFILTER;
	    
	    /* Tell the caller, if they want to know. */
	    if (levelp)
		*levelp = level;
	}
    }
#endif /* HAVE_MULTIFILTERS */
    return 0;
}
