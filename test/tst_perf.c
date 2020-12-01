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

#define FILE_NAME "tst_perf.nc"
#define TEST "tst_perf"
#define STR_LEN 255
#define MAX_LEN 1024
#define X_NAME "X"
#define Y_NAME "Y"
#define NDIM2 2
#define NUM_ELEMENTS 6
#define MAX_NAME_LEN 50
#define ELEMENTS_NAME "Elements"
#define VAR_NAME "Wacky_Woolies"
#define NX 60
#define NY 120
#define DEFLATE_LEVEL 3
#define SIMPLE_VAR_NAME "data"

#define NFILE 2

#define NX_BIG 1000
#define NY_BIG 1000

#define MIN_ZSTD 0
#define MAX_ZSTD 9

/* Create the test file. */
int
create_file(char *file_name, int f, int zstd, int *data_out)
{
    int ncid;
    int dimid[NDIM2];
    int varid;
    
    if (nc_create(file_name, NC_NETCDF4, &ncid)) ERR;
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
    printf("*** Checking Zstandard performance...");
    {
        int *data_out;
        int x, f;

        if (!(data_out = malloc(NX_BIG * NY_BIG * sizeof(int)))) ERR;

        /* Create some data to write. */
        for (x = 0; x < NX_BIG * NY_BIG; x++)
            data_out[x] = x * NY_BIG + x % NX_BIG;

	for (f = 0; f < NFILE; f++)
	{
	    char file_name[STR_LEN + 1];
	    int *data_in;
	    int zstd = 3;
	    
	    if (!(data_in = malloc(NX_BIG * NY_BIG * sizeof(int)))) ERR;
	    
	    sprintf(file_name, "%s_%s.nc", TEST, (f ? "zstandard" : "uncompressed"));
	    nc_set_log_level(3);

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
	} /* next file */
        free(data_out);
    }
    SUMMARIZE_ERR;
#endif /* BUILD_ZSTD */
    FINAL_RESULTS;
}
