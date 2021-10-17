/* Header file for data from the NetCDF Community Codec Repository library.
 *
 * Ed Hartnett
 * 12/24/19
 */
#ifndef _CCR_H
#define _CCR_H

#include <netcdf.h>
#include <netcdf_filter.h>

/** The filter ID for BZIP2 compression. */
#define BZIP2_ID 307

/** The filter ID for LZ4 compression. */
#define LZ4_ID 32004

/** The filter ID for BitGroom quantization. */
#define BITGROOM_ID 32022

/** Number of parameters used internally by filter and returned by nc_inq_var_bitgroom() */
#define BITGROOM_FLT_PRM_NBR 5 /* H5Zbitgroom.c: CCR_FLT_PRM_NBR */

/** The filter ID for BitGroom quantization. */
#define GRANULARBG_ID 37373

/** Number of parameters used internally by filter and returned by nc_inq_var_granularbg() */
#define GRANULARBG_FLT_PRM_NBR 5 /* H5Zgranularbg.c: CCR_FLT_PRM_NBR */

/** The filter ID for Zstandard compression. */
#define ZSTANDARD_ID 32015

/* This macro prints an error message with line number and name of
 * test program, and the netCDF error string. */
#define NC_ERR(stat) do {						\
        fflush(stdout); /* Make sure our stdout is synced with stderr. */ \
        fprintf(stderr, "Sorry! Unexpected result, %s, line: %d %s\n",	\
                __FILE__, __LINE__, nc_strerror(stat));			\
        fflush(stderr);							\
        return 2;							\
    } while (0)

#if defined(__cplusplus)
extern "C" {
#endif

    /* Library prototypes... */
    int nc_def_var_bzip2(int ncid, int varid, int level);
    int nc_inq_var_bzip2(int ncid, int varid, int *bzip2p, int *levelp);
    int nc_def_var_bitgroom(int ncid, int varid, int nsd);
    int nc_inq_var_bitgroom(int ncid, int varid, int *bitgroomp, int *nsdp);
    int nc_def_var_zstandard(int ncid, int varid, int level);
    int nc_inq_var_zstandard(int ncid, int varid, int *zstandardp, int *levelp);
    int nc_def_var_granularbg(int ncid, int varid, int nsd);
    int nc_inq_var_granularbg(int ncid, int varid, int *granularbgp, int *nsdp);

#if defined(__cplusplus)
}
#endif

#endif /* _CCR_H */
