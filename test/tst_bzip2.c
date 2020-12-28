/* This is part of the netCDF package. Copyright 2018 University
   Corporation for Atmospheric Research/Unidata See COPYRIGHT file for
   conditions of use.

   Test HDF5 dataset code, even more. These are not intended to be
   exhaustive tests, but they use HDF5 the same way that netCDF-4
   does, so if these tests don't work, than netCDF-4 won't work
   either.
*/

#include "config.h"
#include "ccr.h"
#include "ccr_test.h"
#include <hdf5.h>
#include <H5DSpublic.h>
#include <netcdf.h>
#include <math.h> /* Define fabs(), powf(), round() */

#define FILE_NAME "tst_bzip2.nc"
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

/* Err is used to keep track of errors within each set of tests,
 * total_err is the number of errors in the entire test program, which
 * generally cosists of several sets of tests. */
static int total_err = 0, err = 0;

int
main()
{
    printf("\n*** Checking HDF5 Bzip2 compression.\n");
    printf("*** Checking simple bzip2 filter...");
    {
        int ncid;
        int dimid[NDIM2];
        int varid;
        int data_out[NX][NY];
        int x, y;
        int level_in, bzip2;

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
        if (nc_def_var(ncid, VAR_NAME, NC_INT, NDIM2, dimid, &varid)) ERR;

        /* These won't work. */
        if (nc_def_var_bzip2(ncid, varid, -9) != NC_EINVAL) ERR;
        if (nc_def_var_bzip2(ncid, varid, 0) != NC_EINVAL) ERR;
        if (nc_def_var_bzip2(ncid, varid, 10) != NC_EINVAL) ERR;

        /* Check setting. */
        if (nc_inq_var_bzip2(ncid, varid, &bzip2, &level_in)) ERR;
        if (bzip2) ERR;

        /* Set up compression. */
        if (nc_def_var_bzip2(ncid, varid, 8)) ERR;

        /* Check setting. */
        if (nc_inq_var_bzip2(ncid, varid, &bzip2, &level_in)) ERR;
        if (!bzip2 || level_in != 8) ERR;
        level_in = 0;
        bzip2 = 1;
        if (nc_inq_var_bzip2(ncid, varid, NULL, &level_in)) ERR;
        if (nc_inq_var_bzip2(ncid, varid, &bzip2, NULL)) ERR;
        if (!bzip2 || level_in != 8) ERR;
        if (nc_inq_var_bzip2(ncid, varid, NULL, NULL)) ERR;

        /* Write the data. */
        if (nc_put_var(ncid, varid, data_out)) ERR;

        /* Close the file. */
        if (nc_close(ncid)) ERR;

        {
            int data_in[NX][NY];

            /* Now reopen the file and check. */
            if (nc_open(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

            /* Check setting. */
            if (nc_inq_var_bzip2(ncid, varid, &bzip2, &level_in)) ERR;
            if (!bzip2 || level_in != 8) ERR;

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
#ifdef BUILD_BITGROOM    
#ifdef HAVE_MULTIFILTERS
    printf("*** Checking bzip2 filter with bitgroom...");
    {
        int ncid;
        int dimid[NDIM2];
        int varid;
        float data_out[NX][NY];
        int x, y;
        int nsd_out = 3;	
      	int bitgroom, nsd_in;
	int level_in, bzip2;

        /* Create some data to write. */
        for (x = 0; x < NX; x++)
            for (y = 0; y < NY; y++)
                data_out[x][y] = (float)x * NY + y / y;

        /* Create file. */
        if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

        /* Create dims. */
        if (nc_def_dim(ncid, X_NAME, NX, &dimid[0])) ERR;
        if (nc_def_dim(ncid, Y_NAME, NY, &dimid[1])) ERR;

        /* Create the variable. */
        if (nc_def_var(ncid, VAR_NAME, NC_FLOAT, NDIM2, dimid, &varid)) ERR;

	/* Bitgroom is not on. */
        if (nc_inq_var_bitgroom(ncid, varid, &bitgroom, &nsd_in)) ERR;
	if (bitgroom) ERR;

        /* These won't work. */
        if (nc_def_var_bitgroom(ncid, varid, -9) != NC_EINVAL) ERR;
        if (nc_def_var_bitgroom(ncid, varid, 0) != NC_EINVAL) ERR;
        if (nc_def_var_bitgroom(ncid, varid, 16) != NC_EINVAL) ERR;

	/* Bitgroom is still not on. */
        if (nc_inq_var_bitgroom(ncid, varid, &bitgroom, &nsd_in)) ERR;
	if (bitgroom) ERR;

        /* Check setting. */
        if (nc_inq_var_bzip2(ncid, varid, &bzip2, &level_in)) ERR;
        if (bzip2) ERR;

	/* Turn on bitgroom. */
	if (nc_def_var_bitgroom(ncid, varid, nsd_out)) ERR;

        /* Set up compression. */
        if (nc_def_var_bzip2(ncid, varid, 8)) ERR;

        /* Check setting. */
        if (nc_inq_var_bzip2(ncid, varid, &bzip2, &level_in)) ERR;
        if (!bzip2 || level_in != 8) ERR;
        level_in = 0;
        bzip2 = 1;
        if (nc_inq_var_bzip2(ncid, varid, NULL, &level_in)) ERR;
        if (nc_inq_var_bzip2(ncid, varid, &bzip2, NULL)) ERR;
        if (!bzip2 || level_in != 8) ERR;
        if (nc_inq_var_bzip2(ncid, varid, NULL, NULL)) ERR;

        /* Write the data. */
        if (nc_put_var(ncid, varid, data_out)) ERR;

        /* Close the file. */
        if (nc_close(ncid)) ERR;

        {
            float data_in[NX][NY];

            /* Now reopen the file and check. */
            if (nc_open(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

            /* Check setting. */
            if (nc_inq_var_bzip2(ncid, varid, &bzip2, &level_in)) ERR;
            if (!bzip2 || level_in != 8) ERR;

            /* Read the data. */
            if (nc_get_var(ncid, varid, data_in)) ERR;

            /* Check the data. */
            for (x = 0; x < NX; x++)
               for (y = 0; y < NY; y++)
		  if (fabs(data_in[x][y] - data_out[x][y]) > 0.5 * fabs(powf(10.0, -nsd_out) * data_out[x][y])) ERR;

            /* Close the file. */
            if (nc_close(ncid)) ERR;
        }
    }
    SUMMARIZE_ERR;
#endif
#endif
    FINAL_RESULTS;
}
