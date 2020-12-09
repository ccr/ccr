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

#define NFILE 10

#define NX_BIG 1000
#define NY_BIG 1000
/* Restore these larger values to run the big file tests. */
/* #define NX_REALLY_BIG 10000 */
/* #define NY_REALLY_BIG 5000 */
#define NX_REALLY_BIG 100
#define NY_REALLY_BIG 50
#define NUM_REC 5

#define MIN_ZSTD 0
#define MAX_ZSTD 9

/* Create the test file. */
int
create_file(char *file_name, int f, int zstd, int *data_out)
{
    int ncid;
    int dimid[NDIM2];
    int varid;
    
    if (nc_create(file_name, NC_CLOBBER|NC_NETCDF4, &ncid)) ERR;
    if (nc_def_dim(ncid, X_NAME, NX_BIG, &dimid[0])) ERR;
    if (nc_def_dim(ncid, Y_NAME, NY_BIG, &dimid[1])) ERR;
    if (nc_def_var(ncid, VAR_NAME, NC_INT, NDIM2, dimid, &varid)) ERR;
    if (f)
	if (nc_def_var_zstandard(ncid, varid, zstd)) ERR;
    if (nc_put_var(ncid, varid, data_out)) ERR;
    if (nc_close(ncid)) ERR;
    return 0;
}
    
int
main()
{
    printf("\n*** Checking Performance of filters.\n");
#ifdef BUILD_ZSTD    
    printf("*** Checking Zstandard performance on small data set...");
    {
        int *data_out;
        int x, f;
	int zstd = MIN_ZSTD;

        if (!(data_out = malloc(NX_BIG * NY_BIG * sizeof(int)))) ERR;

        /* Create some data to write. */
        for (x = 0; x < NX_BIG * NY_BIG; x++)
            data_out[x] = x * NY_BIG + x % NX_BIG;

	for (f = 0; f < NFILE; f++)
	{
	    char file_name[STR_LEN + 1];
	    int *data_in;
	    
	    if (!(data_in = malloc(NX_BIG * NY_BIG * sizeof(int)))) ERR;

	    if (f)
		sprintf(file_name, "%s_zstandard_%d.nc", TEST,  zstd);
	    else
		sprintf(file_name, "%s_uncompressed.nc", TEST);

	    /* Create file. */
	    if (create_file(file_name, f, zstd, data_out)) ERR;
	    
	    /* Check file. */
	    {
		int ncid;
		int varid = 0;
		int level_in, zstandard;
		
		if (nc_open(file_name, NC_NOWRITE, &ncid)) ERR;
		if (nc_inq_var_zstandard(ncid, varid, &zstandard, &level_in)) ERR;
		if (f)
		{
		    if (!zstandard || level_in != zstd) ERR;
		}
		else
		{
		    if (zstandard) ERR;
		}
		if (nc_get_var(ncid, varid, data_in)) ERR;
		for (x = 0; x < NX_BIG * NY_BIG; x++)
		    if (data_in[x] != data_out[x]) ERR;
		if (nc_close(ncid)) ERR;
		free(data_in);
	    }

	    zstd += 1;
	} /* next file */
        free(data_out);
    }
    SUMMARIZE_ERR;
    printf("*** Checking Zstandard performance on large float data set...");
    {
        float *data_out;
        int x, f;
	int zstd = MIN_ZSTD;
	float a = 5.0;
    
        if (!(data_out = malloc(NX_REALLY_BIG * NY_REALLY_BIG * sizeof(int)))) ERR;

	for (f = 0; f < NFILE; f++)
	{
	    char file_name[STR_LEN + 1];
	    float *data_in;
	    int ncid;
	    int dimid[NDIM3];
	    int varid;
	    size_t start[NDIM3] = {0, 0, 0};
	    size_t count[NDIM3] = {1, NX_REALLY_BIG, NY_REALLY_BIG};
	    
	    if (!(data_in = malloc(NX_REALLY_BIG * NY_REALLY_BIG * sizeof(float)))) ERR;

	    if (f)
		sprintf(file_name, "%s_zstandard_really_big_%d.nc", TEST,  zstd);
	    else
		sprintf(file_name, "%s_uncompressed_really_big.nc", TEST);

	    /* Create file. */
	    if (nc_create(file_name, NC_CLOBBER|NC_NETCDF4, &ncid)) ERR;
	    if (nc_def_dim(ncid, EARTHQUAKES_AND_LIGHTNING, NC_UNLIMITED, &dimid[0])) ERR;
	    if (nc_def_dim(ncid, VOICE_OF_RAGE, NX_REALLY_BIG, &dimid[1])) ERR;
	    if (nc_def_dim(ncid, VOICE_OF_RUIN, NY_REALLY_BIG, &dimid[2])) ERR;
	    if (nc_def_var(ncid, VAR_NAME_2, NC_FLOAT, NDIM3, dimid, &varid)) ERR;
	    if (f)
		if (nc_def_var_zstandard(ncid, varid, zstd)) ERR;

	    /* Write data records. */
	    for (start[0] = 0; start[0] < NUM_REC; start[0]++)
	    {
		/* Create a new record to write. */
		for (x = 0; x < NX_REALLY_BIG * NY_REALLY_BIG; x++)
		    data_out[x] = ((float)rand()/(float)(RAND_MAX)) * a;
		
		if (nc_put_vara_float(ncid, varid, start, count, data_out)) ERR;
	    }
	    
	    if (nc_close(ncid)) ERR;
	    
	    /* /\* Check file. *\/ */
	    /* { */
	    /* 	int ncid; */
	    /* 	int varid = 0; */
	    /* 	int level_in, zstandard; */
		
	    /* 	if (nc_open(file_name, NC_NOWRITE, &ncid)) ERR; */
	    /* 	if (nc_inq_var_zstandard(ncid, varid, &zstandard, &level_in)) ERR; */
	    /* 	if (f) */
	    /* 	{ */
	    /* 	    if (!zstandard || level_in != zstd) ERR; */
	    /* 	} */
	    /* 	else */
	    /* 	{ */
	    /* 	    if (zstandard) ERR; */
	    /* 	} */
	    /* 	if (nc_get_var(ncid, varid, data_in)) ERR; */
	    /* 	for (x = 0; x < NX_REALLY_BIG * NY_REALLY_BIG; x++) */
	    /* 	    if (data_in[x] != data_out[x]) ERR; */
	    /* 	if (nc_close(ncid)) ERR; */
	    /* } */

	    zstd += 1;
	    free(data_in);
	} /* next file */
        free(data_out);
    }
    SUMMARIZE_ERR;
#endif /* BUILD_ZSTD */
    FINAL_RESULTS;
}
