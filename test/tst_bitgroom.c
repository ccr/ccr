/* This is part of the CCR package. Copyright 2020.

   Test BitGroom quantization.

   Charlie Zender 9/18/20, Ed Hartnett 1/20/20
*/

#include "config.h"
#include <math.h> /* Define fabs(), powf(), round() */
#include "ccr.h"
#include "ccr_test.h"
#include <hdf5.h>
#include <H5DSpublic.h>
#include <netcdf.h>

#define FILE_NAME "tst_bitgroom.nc"
#define TEST "tst_bitgroom"
#define STR_LEN 255
#define X_NAME "X"
#define Y_NAME "Y"
#define NDIM2 2
#define VAR_NAME "Bad_Moon_Rising"
#define VAR_NAME2 "Fortunate_Son"
#define NDIMS 2
#define NX 60
#define NY 120

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
    printf("\n*** Checking BitGroom filter.\n");
    printf("*** Checking BitGroom quantization...");
    {
        int ncid;
        int dimid[NDIM2];
        int varid, varid2;
        float data_out[NX][NY];
        int x, y;
        int nsd_in;
        int nsd_out=3;
        int bitgroom;

        /* Create some data to write. */
        for (x = 0; x < NX; x++)
            for (y = 0; y < NY; y++)
                data_out[x][y] = x * NY + y;

        /* Create file. */
        if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

        /* Create dims. */
        if (nc_def_dim(ncid, X_NAME, NX, &dimid[0])) ERR;
        if (nc_def_dim(ncid, Y_NAME, NY, &dimid[1])) ERR;

        /* Create the variables. */
        if (nc_def_var(ncid, VAR_NAME, NC_FLOAT, NDIM2, dimid, &varid)) ERR;
        if (nc_def_var(ncid, VAR_NAME2, NC_DOUBLE, NDIM2, dimid, &varid2)) ERR;

        /* These won't work. */
        if (nc_def_var_bitgroom(ncid, varid, -9) != NC_EINVAL) ERR;
        if (nc_def_var_bitgroom(ncid, varid, 0) != NC_EINVAL) ERR;
        if (nc_def_var_bitgroom(ncid, varid, 8) != NC_EINVAL) ERR;
        if (nc_def_var_bitgroom(ncid, varid2, 16) != NC_EINVAL) ERR;

        /* Check setting. */
        if (nc_inq_var_bitgroom(ncid, varid, &bitgroom, &nsd_in)) ERR;
        if (bitgroom) ERR;
        if (nc_inq_var_bitgroom(ncid, varid2, &bitgroom, &nsd_in)) ERR;
        if (bitgroom) ERR;

        /* Set up quantization. */
        if (nc_def_var_bitgroom(ncid, varid, nsd_out)) ERR;
        if (nc_def_var_bitgroom(ncid, varid2, nsd_out)) ERR;

        /* Check setting. */
        if (nc_inq_var_bitgroom(ncid, varid, &bitgroom, &nsd_in)) ERR;
        if (!bitgroom || nsd_in != nsd_out) ERR;
        nsd_in = 0;
        bitgroom = 1;
        if (nc_inq_var_bitgroom(ncid, varid, NULL, &nsd_in)) ERR;
        if (nc_inq_var_bitgroom(ncid, varid, &bitgroom, NULL)) ERR;
        if (!bitgroom || nsd_in != nsd_out) ERR;
        if (nc_inq_var_bitgroom(ncid, varid, NULL, NULL)) ERR;

        /* Check varid2. */
        if (nc_inq_var_bitgroom(ncid, varid2, &bitgroom, &nsd_in)) ERR;
        if (!bitgroom || nsd_in != nsd_out) ERR;

        /* Write the data. */
        if (nc_put_var(ncid, varid, data_out)) ERR;
        if (nc_put_var_float(ncid, varid2, (float *)data_out)) ERR;

        /* Close the file. */
        if (nc_close(ncid)) ERR;

        {
            float data_in[NX][NY];
            float data_in2[NX][NY];
            float data_tst[NX][NY];

            /* Now reopen the file and check. */
            if (nc_open(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

            /* Check settings. */
            if (nc_inq_var_bitgroom(ncid, varid, &bitgroom, &nsd_in)) ERR;
            if (!bitgroom || nsd_in != nsd_out) ERR;
            if (nc_inq_var_bitgroom(ncid, varid2, &bitgroom, &nsd_in)) ERR;
            if (!bitgroom || nsd_in != nsd_out) ERR;

            /* Read the data. */
            if (nc_get_var(ncid, varid, data_in)) ERR;
            if (nc_get_var_float(ncid, varid2, (float *)data_in2)) ERR;

            /* Check the data. Quantization alter data, so do not check for equality :) */
            double scale=powf(10.0,nsd_out);
            for (x = 0; x < NX; x++)
            {
                for (y = 0; y < NY; y++)
                {
                    /* //(void)printf("dat_rgn = %g, dat_bgr = %g, dat_tst = %g\n",data_out[x][y],data_in[x][y],data_tst[x][y]); */
                    data_tst[x][y] = rint(scale * data_out[x][y]) / scale;
                    if (fabs(data_in[x][y] - data_tst[x][y]) > fabs(5.0 * data_out[x][y] / scale)) ERR;
                    if (fabs(data_in2[x][y] - data_tst[x][y]) > fabs(5.0 * data_out[x][y] / scale)) ERR;
                }
            }

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
        float *data_out;
        float *data_in;
        int x, f;
        int nsd_in, bitgroom;
        int nsd_out=3;

        if (!(data_out = malloc(NX_BIG * NY_BIG * sizeof(float)))) ERR;
        if (!(data_in = malloc(NX_BIG * NY_BIG * sizeof(float)))) ERR;

        /* Create some data to write. */
        for (x = 0; x < NX_BIG * NY_BIG; x++)
            data_out[x] = x * NY_BIG + x % NX_BIG;

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
                if (nc_def_var_bitgroom(ncid, varid, nsd_out)) ERR;
            if (nc_put_var(ncid, varid, data_out)) ERR;
            if (nc_close(ncid)) ERR;

            /* Check file. */
            {
                if (nc_open(file_name, NC_NETCDF4, &ncid)) ERR;
                if (nc_inq_var_bitgroom(ncid, varid, &bitgroom, &nsd_in)) ERR;
                if (f)
                {
                    if (!bitgroom || nsd_in != nsd_out) ERR;
                }
                else
                {
                    if (bitgroom) ERR;
                }
                if (nc_get_var(ncid, varid, data_in)) ERR;
                for (x = 0; x < NX_BIG * NY_BIG; x++)
                    /* Check the data. Quantization alter data, so do not check for equality :) */
                    //printf("nsd_in = %d, x = %d, dat_out = %g, dat_in = %g, dff = %g\n",nsd_in,x,data_out[x],data_in[x],fabs(data_in[x]-data_out[x]));
                    if (fabs(data_in[x] - data_out[x]) > 0.5*fabs(powf(10.0,-nsd_in)*data_out[x])) ERR;
                if (nc_close(ncid)) ERR;
            }
        } /* next file */

        free(data_out);
        free(data_in);
    }
    SUMMARIZE_ERR;
#define NTYPES 9
    printf("*** Checking BitGroom handling of non-floats...");
    {
        int ncid;
        int dimid[NDIM2];
        int varid;
        int nsd_in, bitgroom;
        int nsd_out=3;
        char file_name[STR_LEN + 1];
        int xtype[NTYPES] = {NC_CHAR, NC_SHORT, NC_INT, NC_BYTE, NC_UBYTE, NC_USHORT, NC_UINT, NC_INT64, NC_UINT64};
        int t;

        for (t = 0; t < NTYPES; t++)
        {
            sprintf(file_name, "%s_bitgroom_type_%d.nc", TEST, xtype[t]);
            nc_set_log_level(3);

            /* Create file. */
            if (nc_create(file_name, NC_NETCDF4, &ncid)) ERR;
            if (nc_def_dim(ncid, X_NAME, NX_BIG, &dimid[0])) ERR;
            if (nc_def_dim(ncid, Y_NAME, NY_BIG, &dimid[1])) ERR;
            if (nc_def_var(ncid, VAR_NAME, xtype[t], NDIM2, dimid, &varid)) ERR;

            /* Bitgroom filter returns NC_EINVAL because this is not an
             * NC_FLOAT or NC_DOULBE. */
            if (nc_def_var_bitgroom(ncid, varid, nsd_out) != NC_EINVAL) ERR;
            if (nc_close(ncid)) ERR;

            /* Check file. */
            {
                if (nc_open(file_name, NC_NETCDF4, &ncid)) ERR;
                if (nc_inq_var_bitgroom(ncid, varid, &bitgroom, &nsd_in)) ERR;
                if (bitgroom) ERR;
                if (nc_close(ncid)) ERR;
            }
        }
    }
    SUMMARIZE_ERR;
    FINAL_RESULTS;
}
