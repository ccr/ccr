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
 * @image html NetCDF_Filters.png
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
 * LZ4
 *
 * "LZ4 is a lossless data compression algorithm that is focused on
 * compression and decompression speed. It belongs to the LZ77 family
 * of byte-oriented compression schemes" (Wikipedia). For full
 * documentaton see http://lz4.github.io/lz4/. For additional info see
 * https://github.com/lz4/lz4 and
 * https://en.wikipedia.org/wiki/LZ4_(compression_algorithm).
 *
 * In C:
 * - nc_def_var_lz4()
 * - nc_inq_var_lz4()
 *
 * In Fortran:
 * - nf90_def_var_lz4()
 * - nf90_inq_var_lz4()
 *
 * BitGroom
 *
 * The BitGroom filter quantizes the mantissa of floating point values
 * (integers are unaffected) by the appropriate amount to retain not
 * less than the requested Number of Significant Digits (NSD), usually
 * taken as the intrinsic precision of the measured or modeled data.
 * BitGroomed data remain in IEEE-754 format, and are more accurate
 * than other quantization filters such as BitShaving and Bitsetting.
 * Consider BitGroom as a pre-filter for subsequent lossless compression 
 * which with the simple mantissas yields better compression ratios.
 * Zender, C. S. (2016), Bit Grooming: Statistically accurate 
 * precision-preserving quantization with compression, evaluated in 
 * the netCDF Operators (NCO, v4.4.8+), Geosci. Model Dev., 9, 
 * 3199-3211, doi:10.5194/gmd-9-3199-2016.
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
 */

#include "config.h"
#include "ccr.h"
#include <hdf5.h>
#include <H5DSpublic.h>

/**
 * Initialize Community Codec Repository library.
 *
 * @return 0 for success, error code otherwise.
 * @author Ed Hartnett
 */
int
nc_initialize_ccr()
{
    return 0;
}

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
    unsigned int id;
    size_t nparams;
    int bzip2 = 0; /* Is bzip2 in use? */
    int ret;

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

    return 0;
}

/**
 * Turn on lz4 compression for a variable.
 *
 * @param ncid File ID.
 * @param varid Variable ID.
 * @param level From 1 to 9. Set the block size to 100k, 200k ... 900k
 * when compressing. (lz4 default level is 9).
 *
 * @return 0 for success, error code otherwise.
 * @author Ed Hartnett
 */
int
nc_def_var_lz4(int ncid, int varid, int level)
{
    unsigned int cd_value = level;
    int ret;

    /* Level must be between 1 and 9. */
    if (level < 1 || level > 9)
        return NC_EINVAL;

    if (!H5Zfilter_avail(LZ4_ID))
    {
        printf ("lz4 filter not available.\n");
        return NC_EFILTER;
    }

    /* Set up the lz4 filter for this var. */
    if ((ret = nc_def_var_filter(ncid, varid, LZ4_ID, 1, &cd_value)))
        return ret;

    return 0;
}

/**
 * Learn whether lz4 compression is on for a variable, and, if so,
 * the level setting.
 *
 * @param ncid File ID.
 * @param varid Variable ID.
 * @param lz4p Pointer that gets a 0 if lz4 is not in use for this
 * var, and a 1 if it is. Ignored if NULL.
 * @param levelp Pointer that gets the level setting (from 1 to 9), if
 * bzlip2 is in use. Ignored if NULL.
 *
 * @return 0 for success, error code otherwise.
 * @author Ed Hartnett
 */
int
nc_inq_var_lz4(int ncid, int varid, int *lz4p, int *levelp)
{
    unsigned int level;
    unsigned int id;
    size_t nparams;
    int lz4 = 0; /* Is lz4 in use? */
    int ret;

    /* Get filter information. */
    ret = nc_inq_var_filter(ncid, varid, &id, &nparams, &level);
    if (ret == NC_ENOFILTER)
    {
	if (lz4p)
	    *lz4p = 0;
	return 0;
    }
    else if (ret)
	return ret;

    /* Is lz4 in use? */
    if (id == LZ4_ID)
        lz4++;

    /* Does caller want to know if lz4 is in use? */
    if (lz4p)
        *lz4p = lz4;

    /* If lz4 is in use, check parameter. */
    if (lz4)
    {
        /* For lz4, there is one parameter. */
        if (nparams != 1)
            return NC_EFILTER;

        /* Tell the caller, if they want to know. */
        if (levelp)
            *levelp = level;
    }

    return 0;
}

/**
 * Turn on BitGroom quantization for a variable.
 *
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
  /* NB: Internally, the filter requires six elements for cd_value
     However, only the first element, NSD, is required, as the other
     arguments can be and are derived from the dcpl (data_class, datum_size),
     and extra queries of the variable (has_mss_val, mss_val).
     Hence, we expose and require only the minimal number (1) of 
     filter parameters to the netCDF mechanism.
     Everything else should be automagical (knock on wood). */ 
  unsigned int cd_value = nsd;
  int ret;
  
  /* NSD must be between 1 and 15 */
  if (nsd < 1 || nsd > 15)
    return NC_EINVAL;
  
  if (!H5Zfilter_avail(BITGROOM_ID))
    {
      printf ("BitGroom filter not available.\n");
      return NC_EFILTER;
    }
  
  /* Set up the BitGroom filter for this var. */
  if ((ret = nc_def_var_filter(ncid, varid, BITGROOM_ID, BITGROOM_FLT_PRM_NBR, &cd_value)))
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
  unsigned int id;
  size_t nparams;
  int bitgroom = 0; /* Is BitGroom in use? */
  int ret;
  
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
      /* BitGroom has 6 internal parameters.
	 We expose only the first (NSD) through this API because a variable's properties 
	 uniquely determine the remainder and exposing them to users, well, invites disaster */
      //fprintf(stdout,"INFO: nc_inq_var_bitgroom() reports BitGroom filter ID = %d, nparams = %lu, nsd[0] = %d\n",id,nparams,nsd[0]);
      if (nparams != BITGROOM_FLT_PRM_NBR)
	return NC_EFILTER;
      
      /* Tell the caller, if they want to know. */
      if (nsdp)
	*nsdp =(int)nsd[0];
    }
  
  return 0;
}

/**
 * Turn on Zstandard compression for a variable.
 *
 * @param ncid File ID.
 * @param varid Variable ID.
 * @param level From -131072 to 22 (depends on Zstandard version). 
 * when compressing. (Zstandard default level is 3).
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
 * @param levelp Pointer that gets the level setting (from 1 to 9), if
 * bzlip2 is in use. Ignored if NULL.
 *
 * @return 0 for success, error code otherwise.
 * @author Ed Hartnett
 */
int
nc_inq_var_zstandard(int ncid, int varid, int *zstandardp, int *levelp)
{
    unsigned int level;
    unsigned int id;
    size_t nparams;
    int zstandard = 0; /* Is Zstandard in use? */
    int ret;

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

    return 0;
}
