/* This is part of the CCR package. Copyright 2020.

   Test performance by reading an existing climate data file, and
   rewriting some of the data in netCDF/HDF5 with filters.

   Ed Hartnett 12/15/20
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

#define INPUT_FILE "eamv1_ne30np4l72.nc"
#define TEST "tst_eamv1_benchmark"
#define STR_LEN 255

#define NDIM4 4

#define MILLION 1000000
#define NFILE3 3
#define MAX_COMPRESSION_STR 4

#define GRID_XT_SIZE 3072
#define GRID_YT_SIZE 1536
#define PFULL_SIZE 127

#define HGTSFC "hgtsfc"

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
    printf("\n*** Checking Performance of filters.\n");
#ifdef BUILD_ZSTD    
    printf("*** Checking Zstandard vs. zlib performance on large GFS data set...");
    printf("\ncompression, level, write time (s), file size (MB)\n");
    {
        float *data;
	int f;
	    
        if (!(data = malloc(PFULL_SIZE * GRID_XT_SIZE * GRID_YT_SIZE * sizeof(int)))) ERR;
	
	/* Write three files, one uncompressed, one with zlib, and one with zstd. */
	for (f = 0; f < NFILE3; f++)
	{
	    int level = 1;
	    char compression[MAX_COMPRESSION_STR + 1];
	    char file_name[STR_LEN + 1];
	    int ncid_in;
	    int varid_in;
	    int ncid;
	    int varid;
	    int dimid[NDIM4];
	    struct timeval start_time, end_time, diff_time;
	    int meta_write_us;
	    struct stat st;

	    switch (f)
	    {
	    case 0:
		strcpy(compression, "none");
		break;
	    case 1:
		strcpy(compression, "zstd");
		break;
	    case 2:
		strcpy(compression, "zlib");
		break;
	    }

	    /* Determine output filename. */
	    sprintf(file_name, "%s_%s_gfs_%d.nc", TEST, compression, level);

	    /* /\* Open input file. *\/ */
	    /* if (nc_open(INPUT_FILE, NC_NOWRITE, &ncid_in)) ERR; */
	    /* if (nc_inq_varid(ncid_in, HGTSFC, &varid_in)) ERR; */
	
	    /* /\* Read input data. *\/ */
	    /* if (nc_get_var_float(ncid_in, varid_in, data)) ERR; */

	    /* /\* Close input file. *\/ */
	    /* if (nc_close(ncid_in)) ERR; */

	    /* /\* Create output file. *\/ */
	    /* if (nc_create(file_name, NC_CLOBBER|NC_NETCDF4, &ncid)) ERR; */
	    /* if (nc_def_dim(ncid, "time", NC_UNLIMITED, &dimid[0])) ERR; */
	    /* if (nc_def_dim(ncid, "pfull", PFULL_SIZE, &dimid[1])) ERR; */
	    /* if (nc_def_dim(ncid, "grid_xt", GRID_XT_SIZE, &dimid[2])) ERR; */
	    /* if (nc_def_dim(ncid, "grid_yt", GRID_YT_SIZE, &dimid[3])) ERR; */
	    /* if (nc_def_var(ncid, HGTSFC, NC_FLOAT, NDIM4, dimid, &varid)) ERR; */

	    /* switch (f) */
	    /* { */
	    /* case 0: */
	    /* 	/\* no compression *\/ */
	    /* 	break; */
	    /* case 1: */
	    /* 	if (nc_def_var_zstandard(ncid, varid, level)) ERR; */
	    /* 	break; */
	    /* case 2: */
	    /* 	if (nc_def_var_deflate(ncid, varid, 0, 1, level)) ERR; */
	    /* 	break; */
	    /* } */

	    /* Start timer. */
	    if (gettimeofday(&start_time, NULL)) ERR;
	
	    /* /\* Write data. *\/ */
	    /* if (nc_put_var_float(ncid, varid, data)) ERR; */

	    /* if (nc_close(ncid)) ERR; */
	    if (gettimeofday(&end_time, NULL)) ERR;
	    if (nc4_timeval_subtract(&diff_time, &end_time, &start_time)) ERR;
	    meta_write_us = (int)diff_time.tv_sec * MILLION + (int)diff_time.tv_usec;
	    stat(file_name, &st);
	    printf("%s, %d, %.2f, %.2f\n", (f ? compression : "none"), level, (float)meta_write_us/MILLION,
	    	   (float)st.st_size/MILLION);


	    /* level += increment; */
	    /* free(data_out); */
	} /* next file */
	free(data);
    }
    SUMMARIZE_ERR;
#endif /* BUILD_ZSTD */
    FINAL_RESULTS;
}
