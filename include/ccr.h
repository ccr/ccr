/* Header file for data from the NetCDF Community Codec Repository library.
 *
 * Ed Hartnett
 * 12/24/19
 */
#ifndef _CCR_H
#define _CCR_H

#include <netcdf.h>

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

#if defined(__cplusplus)
}
#endif

#endif /* _CCR_H */