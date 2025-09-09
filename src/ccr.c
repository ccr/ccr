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
 * @image html NetCDF_Filters.png
 *
 */

#include "config.h"
#include "ccr.h"
#include <hdf5.h>
#include <H5DSpublic.h>
#include <stdlib.h>


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
 * @param levelp Pointer that gets the acceleration setting (from 1 to 9).
 * Ignored if NULL.
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

