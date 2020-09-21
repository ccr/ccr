/* This is part of the CCR package. Copyright 2020.

   Test BitGroom quantization.

   Charlie Zender 9/18/20, Ed Hartnett 1/20/20
*/

#include <math.h> /* Needed for round() test */

#include "ccr.h"
#include "ccr_test.h"
#include <hdf5.h>
#include <H5DSpublic.h>
#include <netcdf.h>

#define FILE_NAME "tst_bitgroom.nc"
#define TEST "tst_bitgroom"
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

int
main()
{
    printf("\n*** Checking BitGroom filter.\n");
    printf("*** Checking BitGroom quantization...");
    {
        int ncid;
        int dimid[NDIM2];
        int varid;
        float data_out[NX][NY];
        int x, y;
	int nsd_in;
	int params[BITGROOM_FLT_PRM_NBR];
        int bitgroom;

        /* Create some data to write. */
        for (x = 0; x < NX; x++)
            for (y = 0; y < NY; y++)
                data_out[x][y] = x * NY + y;

        if (nc_initialize_ccr()) ERR;

        /* Create file. */
        if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

        /* Create dims. */
        if (nc_def_dim(ncid, X_NAME, NX, &dimid[0]))
	  ;
        if (nc_def_dim(ncid, Y_NAME, NY, &dimid[0]))
	  ;

        /* Create the variable. */
        if (nc_def_var(ncid, VAR_NAME, NC_FLOAT, NDIM2, dimid, &varid)) ERR;

        /* These won't work. */
        if (nc_def_var_bitgroom(ncid, varid, -9) != NC_EINVAL) ERR;
        if (nc_def_var_bitgroom(ncid, varid, 0) != NC_EINVAL) ERR;
        if (nc_def_var_bitgroom(ncid, varid, 16) != NC_EINVAL) ERR;

        /* Check setting. */
        if (nc_inq_var_bitgroom(ncid, varid, &bitgroom, params)) ERR;
        if (bitgroom) ERR;

        /* Set up quantization. */
        if (nc_def_var_bitgroom(ncid, varid, 3)) ERR;

        /* Check setting. */
        if (nc_inq_var_bitgroom(ncid, varid, &bitgroom, params)) ERR;
        nsd_in = params[0];
        if (!bitgroom || nsd_in != 3) ERR;
        nsd_in = 0;
        bitgroom = 1;
        if (nc_inq_var_bitgroom(ncid, varid, NULL, params)) ERR;
        if (nc_inq_var_bitgroom(ncid, varid, &bitgroom, NULL)) ERR;
        nsd_in = params[0];
        if (!bitgroom || nsd_in != 3) ERR;
        if (nc_inq_var_bitgroom(ncid, varid, NULL, NULL)) ERR;

        /* Write the data. */
        if (nc_put_var(ncid, varid, data_out)) ERR;

        /* Close the file. */
        if (nc_close(ncid)) ERR;

        {
            float data_in[NX][NY];

            /* Now reopen the file and check. */
            if (nc_open(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

            /* Check setting. */
            if (nc_inq_var_bitgroom(ncid, varid, &bitgroom, params)) ERR;
	    nsd_in=params[0];
            if (!bitgroom || nsd_in != 3) ERR;

            /* Read the data. */
            if (nc_get_var(ncid, varid, data_in)) ERR;

            /* Check the data. Quantization alter data, so do not check for equality :) */
	    /* fxm: replace this with better test using round((x*10^NSD)/10^NSD) */
            for (x = 0; x < NX; x++)
               for (y = 0; y < NY; y++)
		 if (round(data_in[x][y]) <= data_out[x][y]-2 ||
		     round(data_in[x][y]) >= data_out[x][y]+2 ) ERR;

            /* Close the file. */
            if (nc_close(ncid)) ERR;
        }
    }
    SUMMARIZE_ERR;
    printf("*** Checking BitGroom size of quantization...");
    {
        int ncid;
        int dimid[NDIM2];
        int varid;
        int *data_out;
        int *data_in;
        int x, y, f;
        int nsd_in, bitgroom;

        if (!(data_out = malloc(NX_BIG * NY_BIG * sizeof(int)))) ERR;
        if (!(data_in = malloc(NX_BIG * NY_BIG * sizeof(int)))) ERR;

        /* Create some data to write. */
        for (x = 0; x < NX_BIG * NY_BIG; x++)
            data_out[x] = x * NY_BIG + x % NX_BIG;

        /* if (nc_initialize_ccr()) ERR; */

        for (f = 0; f < NFILE; f++)
        {
            char file_name[STR_LEN + 1];

            sprintf(file_name, "%s_%s.nc", TEST, (f ? "bitgroom" : "unquantized"));
            nc_set_log_level(3);

            /* Create file. */
            if (nc_create(file_name, NC_NETCDF4, &ncid)) ERR;
            if (nc_def_dim(ncid, X_NAME, NX_BIG, &dimid[0]))
	      ;
            if (nc_def_dim(ncid, Y_NAME, NY_BIG, &dimid[1]))
	      ;
            if (nc_def_var(ncid, VAR_NAME, NC_FLOAT, NDIM2, dimid, &varid)) ERR;
            if (f)
                if (nc_def_var_bitgroom(ncid, varid, 3)) ERR;
            if (nc_put_var(ncid, varid, data_out)) ERR;
            if (nc_close(ncid)) ERR;

            /* Check file. */
            {
                if (nc_open(file_name, NC_NETCDF4, &ncid)) ERR;
                if (nc_inq_var_bitgroom(ncid, varid, &bitgroom, &nsd_in)) ERR;
                if (f)
                {
                    if (!bitgroom || nsd_in != 3) ERR;
                }
                else
                {
                    if (bitgroom) ERR;
                }
                if (nc_get_var(ncid, varid, data_in)) ERR;
                for (x = 0; x < NX_BIG * NY_BIG; x++)
		  /* Check the data. Quantization alter data, so do not check for equality :) */
		  /* fxm: replace this with better test using round((x*10^NSD)/10^NSD) */
		  if (data_in[x] == data_out[x]+73 ) ERR;
                if (nc_close(ncid)) ERR;
            }
        } /* next file */

        free(data_out);
        free(data_in);
    }
    SUMMARIZE_ERR;
    FINAL_RESULTS;
}