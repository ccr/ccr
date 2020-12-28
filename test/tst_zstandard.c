/* This is part of the CCR package. Copyright 2020.

   Test Zstandard compression.

   Charlie Zender 9/18/20, Ed Hartnett 1/20/20
*/

#include "config.h"
#include <math.h> /* Define fabs(), powf(), round() */
#include "ccr.h"
#include "ccr_test.h"
#include <hdf5.h>
#include <H5DSpublic.h>
#include <netcdf.h>

#define FILE_NAME "tst_zstandard.nc"
#define TEST "tst_zstandard"
#define STR_LEN 255
#define MAX_LEN 1024
#define X_NAME "X"
#define Y_NAME "Y"
#define NDIM2 2
#define NUM_ELEMENTS 6
#define MAX_NAME_LEN 50
#define ELEMENTS_NAME "Elements"
#define VAR_NAME "Wacky_Woolies"
#define NDIMS 2
#define NX 60
#define NY 120
#define DEFLATE_LEVEL 3
#define SIMPLE_VAR_NAME "data"

#define NFILE 2

#define NX_BIG 100
#define NY_BIG 100

/* Err is used to keep track of errors within each set of tests,
 * total_err is the number of errors in the entire test program, which
 * generally cosists of several sets of tests. */
static int total_err = 0, err = 0;

int
main()
{
    printf("\n*** Checking Zstandard filter.\n");
    printf("*** Checking Zstandard compression...");
    {
        int ncid;
        int dimid[NDIM2];
        int varid;
        float data_out[NX][NY];
        int x, y;
        int level_in, zstandard;

        /* Create some data to write. */
        for (x = 0; x < NX; x++)
            for (y = 0; y < NY; y++)
                data_out[x][y] = x * NY + y;

        /* Create file. */
        if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

        /* Create dims. */
        if (nc_def_dim(ncid, X_NAME, NX, &dimid[0])) ERR;
	if (nc_def_dim(ncid, Y_NAME, NY, &dimid[1])) ERR;

        /* Create the variable. */
        if (nc_def_var(ncid, VAR_NAME, NC_FLOAT, NDIM2, dimid, &varid)) ERR;

        /* These won't work. */
        if (nc_def_var_zstandard(ncid, varid, -131073) != NC_EINVAL) ERR;
        if (nc_def_var_zstandard(ncid, varid, 23) != NC_EINVAL) ERR;

        /* Check setting. */
        if (nc_inq_var_zstandard(ncid, varid, &zstandard, &level_in)) ERR;
        if (zstandard) ERR;

        /* Set up compression. */
        if (nc_def_var_zstandard(ncid, varid, 3)) ERR;

        /* Check setting. */
        if (nc_inq_var_zstandard(ncid, varid, &zstandard, &level_in)) ERR;
        if (!zstandard || level_in != 3) ERR;
        level_in = 0;
        zstandard = 1;
        if (nc_inq_var_zstandard(ncid, varid, NULL, &level_in)) ERR;
        if (nc_inq_var_zstandard(ncid, varid, &zstandard, NULL)) ERR;
        if (!zstandard || level_in != 3) ERR;
        if (nc_inq_var_zstandard(ncid, varid, NULL, NULL)) ERR;

        /* Write the data. */
        if (nc_put_var(ncid, varid, data_out)) ERR;

        /* Close the file. */
        if (nc_close(ncid)) ERR;

        {
            float data_in[NX][NY];

            /* Now reopen the file and check. */
            if (nc_open(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

            /* Check setting. */
            if (nc_inq_var_zstandard(ncid, varid, &zstandard, &level_in)) ERR;
            if (!zstandard || level_in != 3) ERR;

            /* Read the data. */
            if (nc_get_var(ncid, varid, data_in)) ERR;

            /* Check the data. */
            for (x = 0; x < NX; x++)
               for (y = 0; y < NY; y++)
                  if (data_in[x][y] != data_out[x][y]) ERR;

            /* Close the file. */
            if (nc_close(ncid)) ERR;
        }
    }
    SUMMARIZE_ERR;
    printf("*** Checking Zstandard size of compression...");
    {
        int ncid;
        int dimid[NDIM2];
        int varid;
        float *data_out;
        float *data_in;
        int x, f;
        int level_in, zstandard;

        if (!(data_out = malloc(NX_BIG * NY_BIG * sizeof(float)))) ERR;
        if (!(data_in = malloc(NX_BIG * NY_BIG * sizeof(float)))) ERR;

        /* Create some data to write. */
        for (x = 0; x < NX_BIG * NY_BIG; x++)
            data_out[x] = x * NY_BIG + x % NX_BIG;

        for (f = 0; f < NFILE; f++)
        {
            char file_name[STR_LEN + 1];

            sprintf(file_name, "%s_%s.nc", TEST, (f ? "zstandard" : "uncompressed"));
            nc_set_log_level(3);

            /* Create file. */
            if (nc_create(file_name, NC_NETCDF4, &ncid)) ERR;
            if (nc_def_dim(ncid, X_NAME, NX_BIG, &dimid[0])) ERR;
            if (nc_def_dim(ncid, Y_NAME, NY_BIG, &dimid[1])) ERR;
            if (nc_def_var(ncid, VAR_NAME, NC_FLOAT, NDIM2, dimid, &varid)) ERR;
            if (f)
                if (nc_def_var_zstandard(ncid, varid, 3)) ERR;
            if (nc_put_var(ncid, varid, data_out)) ERR;
            if (nc_close(ncid)) ERR;

            /* Check file. */
            {
                if (nc_open(file_name, NC_NETCDF4, &ncid)) ERR;
                if (nc_inq_var_zstandard(ncid, varid, &zstandard, &level_in)) ERR;
                if (f)
                {
                    if (!zstandard || level_in != 3) ERR;
                }
                else
                {
                    if (zstandard) ERR;
                }
                if (nc_get_var(ncid, varid, data_in)) ERR;
                for (x = 0; x < NX_BIG * NY_BIG; x++)
                    if (data_in[x] != data_out[x]) ERR;
                if (nc_close(ncid)) ERR;
            }
        } /* next file */

        free(data_out);
        free(data_in);
    }
    SUMMARIZE_ERR;
#ifdef BUILD_BITGROOM
#ifdef HAVE_MULTIFILTERS
    printf("*** Checking Zstandard size of compression with bitgroom...");
    {
        int ncid;
        int dimid[NDIM2];
        int varid;
        float *data_out;
        float *data_in;
        int x, f;
        int nsd_out = 3;
        int level_in, zstandard;
	int bitgroom, nsd_in;
	int ret;

        if (!(data_out = malloc(NX_BIG * NY_BIG * sizeof(float)))) ERR;
        if (!(data_in = malloc(NX_BIG * NY_BIG * sizeof(float)))) ERR;

        /* Create some data to write. */
        for (x = 0; x < NX_BIG * NY_BIG; x++)
            data_out[x] = x * NY_BIG + x % NX_BIG;

        for (f = 0; f < NFILE; f++)
        {
            char file_name[STR_LEN + 1];

            sprintf(file_name, "%s_%s.nc", TEST, (f ? "bitgroom_zstandard" : "uncompressed"));
            nc_set_log_level(3);

            /* Create file. */
            if (nc_create(file_name, NC_NETCDF4, &ncid)) ERR;
            if (nc_def_dim(ncid, X_NAME, NX_BIG, &dimid[0])) ERR;
            if (nc_def_dim(ncid, Y_NAME, NY_BIG, &dimid[1])) ERR;
            if (nc_def_var(ncid, VAR_NAME, NC_FLOAT, NDIM2, dimid, &varid)) ERR;
            if (f)
	    {
	        if (nc_def_var_bitgroom(ncid, varid, nsd_out)) ERR;
                if (nc_def_var_zstandard(ncid, varid, 3)) ERR;
	    }
            if ((ret = nc_put_var(ncid, varid, data_out)))
		NCERR(ret);
            if (nc_close(ncid)) ERR;

            /* Check file. */
            {
                if (nc_open(file_name, NC_NETCDF4, &ncid)) ERR;
                if ((ret = nc_inq_var_zstandard(ncid, varid, &zstandard, &level_in)))
		    NCERR(ret);
                if (f)
                {
                    if (!zstandard || level_in != 3) ERR;
                }
                else
                {
                    if (zstandard) ERR;
                }
                if ((ret = nc_inq_var_bitgroom(ncid, varid, &bitgroom, &nsd_in)))
		    NCERR(ret);

                if (nc_get_var(ncid, varid, data_in)) ERR;
                for (x = 0; x < NX_BIG * NY_BIG; x++)
		{
		  /* Check the data. Quantization alter data, so do not check for equality :) */
		  /* printf("nsd_out = %d, x = %d, dat_out = %g, dat_in = %g, dff = %g\n",
		     nsd_out,x,data_out[x],data_in[x],fabs(data_in[x]-data_out[x])); */
		  if (fabs(data_in[x] - data_out[x]) > 0.5 * fabs(powf(10.0, -nsd_out) * data_out[x])) ERR;
		}
		    
                if (nc_close(ncid)) ERR;
            }
        } /* next file */

        free(data_out);
        free(data_in);
    }
    SUMMARIZE_ERR;
#endif /* HAVE_MULTIFILTERS */
#endif /* BUILD_BITGROOM */
    FINAL_RESULTS;
}
