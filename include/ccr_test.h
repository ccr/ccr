/**
 * Header file for test code as part of the NetCDF Community Codec
 * Repository.
 *
 * @author Ed Hartnett
 * 12/24/19
 */
#ifndef _CCR_TEST_H
#define _CCR_TEST_H

/* #include "config.h" */
#include "ccr.h"
#include <assert.h>
#include <stdlib.h>

/* Err is used to keep track of errors within each set of tests,
 * total_err is the number of errors in the entire test program, which
 * generally cosists of several sets of tests. */
static int total_err = 0, err = 0;

/* This macro prints an error message with line number and name of
 * test program. */
#define ERR do {							\
        fflush(stdout); /* Make sure our stdout is synced with stderr. */ \
        err++;                                                          \
        fprintf(stderr, "Sorry! Unexpected result, %s, line: %d\n",	\
                __FILE__, __LINE__);					\
        fflush(stderr);							\
        return 2;							\
    } while (0)

/* This macro prints an error message with line number and name of
 * test program. */
#define ERR2 do {							\
        fflush(stdout); /* Make sure our stdout is synced with stderr. */ \
        fprintf(stderr, "Sorry! Unexpected result, %s, line: %d\n",	\
                __FILE__, __LINE__);					\
        fflush(stderr);							\
        return 2;							\
    } while (0)

/* After a set of tests, report the number of errors, and increment
 * total_err. */
#define SUMMARIZE_ERR do { \
   if (err) \
   { \
      printf("%d failures\n", err); \
      total_err += err; \
      err = 0; \
   } \
   else \
      printf("ok.\n"); \
} while (0)

/* This macro prints out our total number of errors, if any, and exits
 * with a 0 if there are not, or a 2 if there were errors. Make will
 * stop if a non-zero value is returned from a test program. */
#define FINAL_RESULTS do { \
   if (total_err) \
   { \
      printf("%d errors detected! Sorry!\n", total_err); \
      return 2; \
   } \
   printf("*** Tests successful!\n"); \
   return 0; \
} while (0)

#endif /* _CCR_TEST_H */
