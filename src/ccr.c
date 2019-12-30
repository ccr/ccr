/* This is the code file for the Community Codex Repository library for netCDF.
 *
 * Ed Hartnett
 * 12/30/19
 */

#include "ccr.h"
#include <hdf5.h>
#include <H5DSpublic.h>

#define H5Z_FILTER_BZIP2 307

size_t H5Z_filter_bzip2(unsigned int flags, size_t cd_nelmts,
                        const unsigned int cd_values[], size_t nbytes,
                        size_t *buf_size, void **buf);

size_t H5Z_filter_lz4(unsigned int flags, size_t cd_nelmts,
                      const unsigned int cd_values[], size_t nbytes,
                      size_t *buf_size, void **buf);
/**
 * Initialize Community Codec Repository library.
 *
 * @return 0 for success, error code otherwise.
 * @author Ed Hartnett
 */
int
nc_initialize_ccr()
{
    const H5Z_class2_t H5Z_BZIP2[1] = {{
            H5Z_CLASS_T_VERS,       /* H5Z_class_t version */
            (H5Z_filter_t)H5Z_FILTER_BZIP2,         /* Filter id number             */
            1,              /* encoder_present flag (set to true) */
            1,              /* decoder_present flag (set to true) */
            "bzip2",                  /* Filter name for debugging    */
            NULL,                       /* The "can apply" callback     */
            NULL,                       /* The "set local" callback     */
            (H5Z_func_t)H5Z_filter_bzip2,         /* The actual filter function   */
        }};
    const H5Z_class2_t H5Z_LZ4[1] = {{
            H5Z_CLASS_T_VERS,       /* H5Z_class_t version */
            (H5Z_filter_t)LZ4_ID,         /* Filter id number             */
            1,              /* encoder_present flag (set to true) */
            1,              /* decoder_present flag (set to true) */
            "lz4",                  /* Filter name for debugging    */
            NULL,                       /* The "can apply" callback     */
            NULL,                       /* The "set local" callback     */
            (H5Z_func_t)H5Z_filter_lz4,         /* The actual filter function   */
        }};

    /* char plugin_path[MAX_LEN + 1]; */
    /* if (H5PLget(0, plugin_path, MAX_LEN) < 0) ERR; */
    /* printf("plugin_path %s\n", plugin_path); */

    if (H5Zregister(H5Z_BZIP2) < 0)
        return NC_EFILTER;

    if (H5Zregister(H5Z_LZ4) < 0)
        return NC_EFILTER;

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
 * bzlip2 is in use. Ignored if NULL.
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
    if ((ret = nc_inq_var_filter(ncid, varid, &id, &nparams, &level)))
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
