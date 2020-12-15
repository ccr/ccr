/* This is part of the CCR package. Copyright 2020.

   Test performance.

   Ed Hartnett 12/1/20
*/

#include "config.h"
#include "ccr.h"
#include "ccr_test.h"
#include <hdf5.h>
#include <H5DSpublic.h>
#include <netcdf.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h> /* Extra high precision time info. */
#include <sys/stat.h> /* To get file sizes. */

#define FILE_NAME "tst_perf.nc"
#define TEST "tst_perf"
#define STR_LEN 255
#define MAX_LEN 1024
#define X_NAME "X"
#define Y_NAME "Y"
#define NDIM2 2
#define NDIM3 3
#define VAR_NAME "Midnight_Special"
#define VAR_NAME_2 "Bad_Moon_Rising"
#define VOICE_OF_RAGE "VOICE_OF_RAGE"
#define VOICE_OF_RUIN "VOICE_OF_RUIN"
#define EARTHQUAKES_AND_LIGHTNING "EARTHQUAKES_AND_LIGHTNING"

#define NFILE 18 /* This must be an even number. */
#define MAX_COMPRESSION_STR 4

#define NX_BIG 1000
#define NY_BIG 1000
/* Restore these larger values to run the big file tests. */
#define NX_REALLY_BIG 1000
#define NY_REALLY_BIG 5000
/* #define NX_REALLY_BIG 100 */
/* #define NY_REALLY_BIG 50 */
#define NUM_REC 5

#define MIN_ZSTD 0
#define MAX_ZSTD 9
#define MIN_ZLIB 1

/* Err is used to keep track of errors within each set of tests,
 * total_err is the number of errors in the entire test program, which
 * generally cosists of several sets of tests. */
static int total_err = 0, err = 0;

/* Prototype from tst_utils.c. */
int nc4_timeval_subtract(struct timeval *result, struct timeval *x,
                         struct timeval *y);

int
main()
{
    struct stat st;
    
    printf("\n*** Checking Performance of filters.\n");
#ifdef BUILD_ZSTD    
    printf("*** Checking Zstandard vs. zlib performance on large float data set...");
    printf("\ncompression, level, write time (s), file size (MB)\n");
    {
        float *data_out;
        size_t x;
	int f;
	int level = MIN_ZSTD;
	char compression[MAX_COMPRESSION_STR + 1];
	float a = 5.0;
    
        if (!(data_out = malloc(NX_REALLY_BIG * NY_REALLY_BIG * sizeof(int)))) ERR;

	/* We will write NFILE compressed file, and 1 uncompressed
	 * file. Half the compressed files will be zstd, half zlib. */
	for (f = 0; f < NFILE + 1; f++)
	{
	    char file_name[STR_LEN + 1];
	    float *data_in;
	    int ncid;
	    int dimid[NDIM3];
	    int varid;
	    size_t start[NDIM3] = {0, 0, 0};
	    size_t count[NDIM3] = {1, NX_REALLY_BIG, NY_REALLY_BIG};
	    struct timeval start_time, end_time, diff_time;
	    int meta_write_us;
	    int increment = 1;
	    
	    if (!(data_in = malloc(NX_REALLY_BIG * NY_REALLY_BIG * sizeof(float)))) ERR;

	    if (f)
		strcpy(compression, (f < NFILE / 2 + 1) ? "zstd" : "zlib");

	    if (f)
		sprintf(file_name, "%s_%s_really_big_%d.nc", TEST, compression, level);
	    else
		sprintf(file_name, "%s_uncompressed_really_big.nc", TEST);

	    /* Create file. */
	    if (gettimeofday(&start_time, NULL)) ERR;

	    if (nc_create(file_name, NC_CLOBBER|NC_NETCDF4, &ncid)) ERR;
	    if (nc_def_dim(ncid, EARTHQUAKES_AND_LIGHTNING, NC_UNLIMITED, &dimid[0])) ERR;
	    if (nc_def_dim(ncid, VOICE_OF_RAGE, NX_REALLY_BIG, &dimid[1])) ERR;
	    if (nc_def_dim(ncid, VOICE_OF_RUIN, NY_REALLY_BIG, &dimid[2])) ERR;
	    if (nc_def_var(ncid, VAR_NAME_2, NC_FLOAT, NDIM3, dimid, &varid)) ERR;
	    if (f)
	    {
		if (f == NFILE / 2 + 1)
		    level = MIN_ZLIB;
		if (f < NFILE / 2 + 1)
		{
		    if (nc_def_var_zstandard(ncid, varid, level)) ERR;
		    increment = 2;
		}
		else
		{
		    if (nc_def_var_deflate(ncid, varid, 0, 1, level)) ERR;
		    increment = 1;
		}
	    }

	    /* Write data records. */
	    for (start[0] = 0; start[0] < NUM_REC; start[0]++)
	    {
		/* Create a new record to write. */
		for (x = 0; x < NX_REALLY_BIG * NY_REALLY_BIG; x++)
		{
		    data_out[x] = ((float)rand()/(float)(RAND_MAX)) * a;
		    /* data_out[x] = x + 1.; */
		}
		
		if (nc_put_vara_float(ncid, varid, start, count, data_out)) ERR;
	    }
	    
	    if (nc_close(ncid)) ERR;
	    if (gettimeofday(&end_time, NULL)) ERR;
	    if (nc4_timeval_subtract(&diff_time, &end_time, &start_time)) ERR;
	    meta_write_us = (int)diff_time.tv_sec * MILLION + (int)diff_time.tv_usec;
	    stat(file_name, &st);
	    printf("%s, %d, %.2f, %.2f\n", (f ? compression : "none"), level, (float)meta_write_us/MILLION,
		   (float)st.st_size/MILLION);
	    
	    /* /\* Check file. *\/ */
	    /* { */
	    /* 	int ncid; */
	    /* 	int varid = 0; */
	    /* 	int level_in, zstandard; */
		
	    /* 	if (nc_open(file_name, NC_NOWRITE, &ncid)) ERR; */
	    /* 	if (nc_inq_var_zstandard(ncid, varid, &zstandard, &level_in)) ERR; */
	    /* 	if (f) */
	    /* 	{ */
	    /* 	    if (f < NFILE / 2 + 1) */
	    /* 	    { */
	    /* 		if (!zstandard) ERR; */
	    /* 	    } */
	    /* 	    else */
	    /* 	    { */
	    /* 	    } */
	    /* 	} */
	    /* 	else */
	    /* 	{ */
	    /* 	    if (zstandard) ERR; */
	    /* 	} */
	    /* 	for (start[0] = 0; start[0] < NUM_REC; start[0]++) */
	    /* 	{ */
	    /* 	    if (nc_get_vara_float(ncid, varid, start, count, data_in)) ERR; */
	    /* 	    for (x = 0; x < NX_REALLY_BIG * NY_REALLY_BIG; x++) */
	    /* 		if (data_in[x] != x + 1.) */
	    /* 		{ */
	    /* 		    printf("data_out[%ld] %g data_in[%ld] %g\n", x, data_out[x], x, data_in[x]); */
	    /* 		    ERR; */
	    /* 		} */
	    /* 	} */
	    /* 	if (nc_close(ncid)) ERR; */
	    /* } */

	    level += increment;
	    free(data_in);
	} /* next file */
        free(data_out);
    }
    SUMMARIZE_ERR;
#endif /* BUILD_ZSTD */
    FINAL_RESULTS;
}
