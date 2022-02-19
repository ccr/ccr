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

#define DIM_LEN_5 5
#define DIM_LEN_8 8
#define NDIM1 1

/* Err is used to keep track of errors within each set of tests,
 * total_err is the number of errors in the entire test program, which
 * generally cosists of several sets of tests. */
static int total_err = 0, err = 0;

/* This var used to help print a float in hex. */
char pf_str[20];

/* This struct allows us to treat float as uint32_t
 * types. */
union FU {
    float f;
    uint32_t u;
};

/* This struct allows us to treat double points as uint64_t
 * types. */
union DU {
    double d;
    uint64_t u;
};

/* This function prints a float as hex. */
char *
pf(float myf)
{
    union {
	float f;
	uint32_t u;
    } fu;
    fu.f = myf;
    sprintf(pf_str, "0x%x", fu.u);
    return pf_str;
}

/* This function prints a double as hex. */
char *
pd(double myd)
{
    union {
	double d;
	uint64_t u;
    } du;
    du.d = myd;
    sprintf(pf_str, "0x%llx", du.u);
    return pf_str;
}

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
            if (nc_def_dim(ncid, X_NAME, NX_BIG, &dimid[0])) ERR;
            if (nc_def_dim(ncid, Y_NAME, NY_BIG, &dimid[1])) ERR;
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
    printf("*** Checking BitGroom values...");
    {
        int ncid;
        int dimid;
        int varid, varid2;
        float float_data[DIM_LEN_5] = {1.11111111, 1.0, 9.99999999, 12345.67, .1234567};
        double double_data[DIM_LEN_5] = {1.1111111, 1.0, 9.999999999, 1234567890.12345, 123456789012345.0};
        int nsd_out = 3;

        /* Create file. */
        if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

        /* Create dims. */
        if (nc_def_dim(ncid, X_NAME, DIM_LEN_5, &dimid)) ERR;

        /* Create the variables. */
        if (nc_def_var(ncid, VAR_NAME, NC_FLOAT, NDIM1, &dimid, &varid)) ERR;
        if (nc_def_var(ncid, VAR_NAME2, NC_DOUBLE, NDIM1, &dimid, &varid2)) ERR;

        /* Set up quantization. */
        if (nc_def_var_bitgroom(ncid, varid, nsd_out)) ERR;
        if (nc_def_var_bitgroom(ncid, varid2, nsd_out)) ERR;

        /* Write the data. */
        if (nc_put_var_float(ncid, varid, float_data)) ERR;
        if (nc_put_var_double(ncid, varid2, double_data)) ERR;

        /* Close the file. */
        if (nc_close(ncid)) ERR;

        {
            float float_data_in[DIM_LEN_5];
            double double_data_in[DIM_LEN_5];
	    int x;

            /* Now reopen the file and check. */
            if (nc_open(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

            /* Read the data. */
            if (nc_get_var_float(ncid, varid, float_data_in)) ERR;
            if (nc_get_var_double(ncid, varid2, double_data_in)) ERR;

	    union FU fin;
	    /* union FU fout; */
	    union FU xpect[DIM_LEN_5];
	    union DU dfin;
	    /* union DU dfout; */
	    union DU double_xpect[DIM_LEN_5];
	    xpect[0].u = 0x3f8e3000;
	    xpect[1].u = 0x3f800fff;
	    xpect[2].u = 0x41200000;
	    xpect[3].u = 0x4640efff;
	    xpect[4].u = 0x3dfcd000;
	    double_xpect[0].u = 0x3ff1c60000000000;
	    double_xpect[1].u = 0x3ff001ffffffffff;
	    double_xpect[2].u = 0x4023fe0000000000;
	    double_xpect[3].u = 0x41d265ffffffffff;
	    double_xpect[4].u = 0x42dc120000000000;

	    for (x = 0; x < DIM_LEN_5; x++)
	    {
		/* fout.f = float_data[x]; */
		fin.f = float_data_in[x];
		/* dfout.d = double_data[x]; */
		dfin.d = double_data_in[x];
		/* printf ("double_data %d : %15g   : %s  double_data_in %d : %15g   : 0x%lx" */
		/* 	" expected %15g   : 0x%lx\n", */
		/* 	x, double_data[x], pd(double_data[x]), x, double_data_in[x], dfin.u, */
		/* 	double_xpect[x].d, double_xpect[x].u); */
		if (fin.u != xpect[x].u)
		    ERR;
		if (dfin.u != double_xpect[x].u)
		    ERR;
	    }

            /* Close the file. */
            if (nc_close(ncid)) ERR;
        }
    }
    SUMMARIZE_ERR;
    printf("*** Checking BitGroom values with type conversion between double and float...");
    {
        int ncid;
        int dimid;
        int varid, varid2;
        float float_data[DIM_LEN_5] = {1.11111111, 1.0, 9.99999999, 12345.67, .1234567};
        double double_data[DIM_LEN_5] = {1.1111111, 1.0, 9.999999999, 1234567890.12345, 123456789012345.0};
        int nsd_out = 3;

        /* Create file. */
        if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

        /* Create dims. */
        if (nc_def_dim(ncid, X_NAME, DIM_LEN_5, &dimid)) ERR;

        /* Create the variables. */
        if (nc_def_var(ncid, VAR_NAME, NC_FLOAT, NDIM1, &dimid, &varid)) ERR;
        if (nc_def_var(ncid, VAR_NAME2, NC_DOUBLE, NDIM1, &dimid, &varid2)) ERR;

        /* Set up quantization. */
        if (nc_def_var_bitgroom(ncid, varid, nsd_out)) ERR;
        if (nc_def_var_bitgroom(ncid, varid2, nsd_out)) ERR;

        /* Write the float data to double, and vice versa. */
        if (nc_put_var_double(ncid, varid, double_data)) ERR;
        if (nc_put_var_float(ncid, varid2, float_data)) ERR;

        /* Close the file. */
        if (nc_close(ncid)) ERR;

        {
            float float_data_in[DIM_LEN_5];
            double double_data_in[DIM_LEN_5];
	    int x;

            /* Now reopen the file and check. */
            if (nc_open(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

            /* Read the data. */
            if (nc_get_var_float(ncid, varid, float_data_in)) ERR;
            if (nc_get_var_double(ncid, varid2, double_data_in)) ERR;

	    union FU fin;
	    /* union FU fout; */
	    union FU xpect[DIM_LEN_5];
	    union DU dfin;
	    /* union DU dfout; */
	    union DU double_xpect[DIM_LEN_5];
	    xpect[0].u = 0x3f8e3000;
	    xpect[1].u = 0x3f800fff;
	    xpect[2].u = 0x41200000;
	    xpect[3].u = 0x4e932fff;
	    xpect[4].u = 0x56e09000;
	    double_xpect[0].u = 0x3ff1c60000000000;
	    double_xpect[1].u = 0x3ff001ffffffffff;
	    double_xpect[2].u = 0x4024000000000000;
	    double_xpect[3].u = 0x40c81dffffffffff;
	    double_xpect[4].u = 0x3fbf9a0000000000;

	    for (x = 0; x < DIM_LEN_5; x++)
	    {
		/* fout.f = float_data[x]; */
		fin.f = float_data_in[x];
		/* dfout.d = double_data[x]; */
		dfin.d = double_data_in[x];
		/* printf ("double_data %d : %15g   : %s  float_data_in %d : %15g   : 0x%x" */
		/* 	" expected %15g   : 0x%x\n", */
		/* 	x, double_data[x], pd(double_data[x]), x, float_data_in[x], fin.u, */
		/* 	xpect[x].f, xpect[x].u); */
		/* printf ("float_data %d : %15g   : %s  double_data_in %d : %15g   : 0x%lx" */
		/* 	" expected %15g   : 0x%x\n", */
		/* 	x, float_data[x], pf(float_data[x]), x, double_data_in[x], dfin.u, */
		/* 	xpect[x].f, xpect[x].u); */
		if (fin.u != xpect[x].u)
		    ERR;
		if (dfin.u != double_xpect[x].u)
		    ERR;
	    }

            /* Close the file. */
            if (nc_close(ncid)) ERR;
        }
    }
    SUMMARIZE_ERR;
    printf("*** Checking BitGroom values with some default fill values...");
    {
        int ncid;
        int dimid;
        int varid, varid2;
        float float_data[DIM_LEN_5] = {1.11111111, NC_FILL_FLOAT, 9.99999999, 12345.67, NC_FILL_FLOAT};
        double double_data[DIM_LEN_5] = {1.1111111, NC_FILL_DOUBLE, 9.999999999, 1234567890.12345, NC_FILL_DOUBLE};
        int nsd_out = 3;

        /* Create file. */
        if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

        /* Create dims. */
        if (nc_def_dim(ncid, X_NAME, DIM_LEN_5, &dimid)) ERR;

        /* Create the variables. */
        if (nc_def_var(ncid, VAR_NAME, NC_FLOAT, NDIM1, &dimid, &varid)) ERR;
        if (nc_def_var(ncid, VAR_NAME2, NC_DOUBLE, NDIM1, &dimid, &varid2)) ERR;

        /* Set up quantization. */
        if (nc_def_var_bitgroom(ncid, varid, nsd_out)) ERR;
        if (nc_def_var_bitgroom(ncid, varid2, nsd_out)) ERR;

        /* Write the data. */
        if (nc_put_var_float(ncid, varid, float_data)) ERR;
        if (nc_put_var_double(ncid, varid2, double_data)) ERR;

        /* Close the file. */
        if (nc_close(ncid)) ERR;

        {
            float float_data_in[DIM_LEN_5];
            double double_data_in[DIM_LEN_5];
	    int x;

            /* Now reopen the file and check. */
            if (nc_open(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

            /* Read the data. */
            if (nc_get_var_float(ncid, varid, float_data_in)) ERR;
            if (nc_get_var_double(ncid, varid2, double_data_in)) ERR;

	    union FU fin;
	    /* union FU fout; */
	    union FU xpect[DIM_LEN_5];
	    union DU dfin;
	    /* union DU dfout; */
	    union DU double_xpect[DIM_LEN_5];
	    xpect[0].u = 0x3f8e3000;
	    xpect[1].u = 0x7cf00000;
	    xpect[2].u = 0x41200000;
	    xpect[3].u = 0x4640efff;
	    xpect[4].u = 0x7cf00000;
	    double_xpect[0].u = 0x3ff1c60000000000;
	    double_xpect[1].u = 0x479e000000000000;
	    double_xpect[2].u = 0x4023fe0000000000;
	    double_xpect[3].u = 0x41d265ffffffffff;
	    double_xpect[4].u = 0x479e000000000000;

	    for (x = 0; x < DIM_LEN_5; x++)
	    {
		/* fout.f = float_data[x]; */
		fin.f = float_data_in[x];
		/* dfout.d = double_data[x]; */
		dfin.d = double_data_in[x];
		/* printf ("float_data %d : %15g   : %s  float_data_in %d : %15g   : 0x%x" */
		/* 	" expected %15g   : 0x%x\n", */
		/* 	x, float_data[x], pf(float_data[x]), x, float_data_in[x], fin.u, */
		/* 	xpect[x].f, xpect[x].u); */
		/* printf ("double_data %d : %15g   : %s  double_data_in %d : %15g   : 0x%lx" */
		/* 	" expected %15g   : 0x%lx\n", */
		/* 	x, double_data[x], pd(double_data[x]), x, double_data_in[x], dfin.u, */
		/* 	double_xpect[x].d, double_xpect[x].u); */
		if (fin.u != xpect[x].u)
		    ERR;
		if (dfin.u != double_xpect[x].u)
		    ERR;
	    }

            /* Close the file. */
            if (nc_close(ncid)) ERR;
        }
    }
    SUMMARIZE_ERR;
    printf("*** Checking BitGroom values with some custom fill values...");
    {
	#define CUSTOM_FILL_FLOAT 99.99999
	#define CUSTOM_FILL_DOUBLE -99999.99999
        int ncid;
        int dimid;
        int varid, varid2;
        float float_data[DIM_LEN_5] = {1.11111111, CUSTOM_FILL_FLOAT, 9.99999999, 12345.67, CUSTOM_FILL_FLOAT};
        double double_data[DIM_LEN_5] = {1.1111111, CUSTOM_FILL_DOUBLE, 9.999999999, 1234567890.12345, CUSTOM_FILL_DOUBLE};
        int nsd_out = 3;
	float custom_fill_float = CUSTOM_FILL_FLOAT;
	double custom_fill_double = CUSTOM_FILL_DOUBLE;

        /* Create file. */
        if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

        /* Create dims. */
        if (nc_def_dim(ncid, X_NAME, DIM_LEN_5, &dimid)) ERR;

        /* Create the variables. */
        if (nc_def_var(ncid, VAR_NAME, NC_FLOAT, NDIM1, &dimid, &varid)) ERR;
	if (nc_put_att_float(ncid, varid, _FillValue, NC_FLOAT, 1, &custom_fill_float)) ERR;
        if (nc_def_var(ncid, VAR_NAME2, NC_DOUBLE, NDIM1, &dimid, &varid2)) ERR;
	if (nc_put_att_double(ncid, varid2, _FillValue, NC_DOUBLE, 1, &custom_fill_double)) ERR;

        /* Set up quantization. */
        if (nc_def_var_bitgroom(ncid, varid, nsd_out)) ERR;
        if (nc_def_var_bitgroom(ncid, varid2, nsd_out)) ERR;

        /* Write the data. */
        if (nc_put_var_float(ncid, varid, float_data)) ERR;
        if (nc_put_var_double(ncid, varid2, double_data)) ERR;

        /* Close the file. */
        if (nc_close(ncid)) ERR;

        {
            float float_data_in[DIM_LEN_5];
            double double_data_in[DIM_LEN_5];
	    int x;

            /* Now reopen the file and check. */
            if (nc_open(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

            /* Read the data. */
            if (nc_get_var_float(ncid, varid, float_data_in)) ERR;
            if (nc_get_var_double(ncid, varid2, double_data_in)) ERR;

	    union FU fin;
	    /* union FU fout; */
	    union FU xpect[DIM_LEN_5];
	    union DU dfin;
	    /* union DU dfout; */
	    union DU double_xpect[DIM_LEN_5];
	    xpect[0].u = 0x3f8e3000;
	    xpect[1].u = 0x42c7ffff;
	    xpect[2].u = 0x41200000;
	    xpect[3].u = 0x4640efff;
	    xpect[4].u = 0x42c7ffff;
	    double_xpect[0].u = 0x3ff1c60000000000;
	    double_xpect[1].u = 0xc0f869fffff583a5;
	    double_xpect[2].u = 0x4023fe0000000000;
	    double_xpect[3].u = 0x41d265ffffffffff;
	    double_xpect[4].u = 0xc0f869fffff583a5;

	    for (x = 0; x < DIM_LEN_5; x++)
	    {
		/* fout.f = float_data[x]; */
		fin.f = float_data_in[x];
		/* dfout.d = double_data[x]; */
		dfin.d = double_data_in[x];
		/* printf ("float_data %d : %15g   : %s  float_data_in %d : %15g   : 0x%x" */
		/* 	" expected %15g   : 0x%x\n", */
		/* 	x, float_data[x], pf(float_data[x]), x, float_data_in[x], fin.u, */
		/* 	xpect[x].f, xpect[x].u); */
		/* printf ("double_data %d : %15g   : %s  double_data_in %d : %15g   : 0x%lx" */
		/* 	" expected %15g   : 0x%lx\n", */
		/* 	x, double_data[x], pd(double_data[x]), x, double_data_in[x], dfin.u, */
		/* 	double_xpect[x].d, double_xpect[x].u); */
		if (fin.u != xpect[x].u)
		    ERR;
		if (dfin.u != double_xpect[x].u)
		    ERR;
	    }

            /* Close the file. */
            if (nc_close(ncid)) ERR;
        }
    }
    SUMMARIZE_ERR;
    printf("*** Checking BitGroom values with type conversion between ints and floats...");
    {
        int ncid;
        int dimid;
        int varid1, varid2;
	unsigned char uc = 99;
	signed char sc = -99;
	unsigned short us = 9999;
	signed short ss = -9999;
	unsigned int ui = 9999999;
	signed int si = -9999999;
	unsigned long long int ull = 999999999;
	signed long long int sll = -999999999;
	size_t index;
        int nsd_out = 3;

        /* Create file. */
        if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

        /* Create dims. */
        if (nc_def_dim(ncid, X_NAME, DIM_LEN_8, &dimid)) ERR;

        /* Create the variables. */
        if (nc_def_var(ncid, VAR_NAME, NC_FLOAT, NDIM1, &dimid, &varid1)) ERR;
        if (nc_def_var(ncid, VAR_NAME2, NC_DOUBLE, NDIM1, &dimid, &varid2)) ERR;

        /* Set up quantization. */
        if (nc_def_var_bitgroom(ncid, varid1, nsd_out)) ERR;
        if (nc_def_var_bitgroom(ncid, varid2, nsd_out)) ERR;

        /* Write data. */
	index = 0;
        if (nc_put_var1_uchar(ncid, varid1, &index, &uc)) ERR;
        if (nc_put_var1_uchar(ncid, varid2, &index, &uc)) ERR;
	index = 1;
        if (nc_put_var1_schar(ncid, varid1, &index, &sc)) ERR;
        if (nc_put_var1_schar(ncid, varid2, &index, &sc)) ERR;
	index = 2;
        if (nc_put_var1_ushort(ncid, varid1, &index, &us)) ERR;
        if (nc_put_var1_ushort(ncid, varid2, &index, &us)) ERR;
	index = 3;
        if (nc_put_var1_short(ncid, varid1, &index, &ss)) ERR;
        if (nc_put_var1_short(ncid, varid2, &index, &ss)) ERR;
	index = 4;
        if (nc_put_var1_uint(ncid, varid1, &index, &ui)) ERR;
        if (nc_put_var1_uint(ncid, varid2, &index, &ui)) ERR;
	index = 5;
        if (nc_put_var1_int(ncid, varid1, &index, &si)) ERR;
        if (nc_put_var1_int(ncid, varid2, &index, &si)) ERR;
	index = 6;
        if (nc_put_var1_ulonglong(ncid, varid1, &index, &ull)) ERR;
        if (nc_put_var1_ulonglong(ncid, varid2, &index, &ull)) ERR;
	index = 7;
        if (nc_put_var1_longlong(ncid, varid1, &index, &sll)) ERR;
        if (nc_put_var1_longlong(ncid, varid2, &index, &sll)) ERR;

        /* Close the file. */
        if (nc_close(ncid)) ERR;

        {
            float float_data_in[DIM_LEN_8];
            double double_data_in[DIM_LEN_8];
	    int x;

            /* Now reopen the file and check. */
            if (nc_open(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

            /* Read the data. */
            if (nc_get_var_float(ncid, varid1, float_data_in)) ERR;
            if (nc_get_var_double(ncid, varid2, double_data_in)) ERR;

	    union FU fin;
	    /* union FU fout; */
	    union FU xpect[DIM_LEN_8];
	    union DU dfin;
	    /* union DU dfout; */
	    union DU double_xpect[DIM_LEN_8];
	    xpect[0].u = 0x42c60000;
	    xpect[1].u = 0xc2c60fff;
	    xpect[2].u = 0x461c3000;
	    xpect[3].u = 0xc61c3fff;
	    xpect[4].u = 0x4b189000;
	    xpect[5].u = 0xcb189fff;
	    xpect[6].u = 0x4e6e6000;
	    xpect[7].u = 0xce6e6fff;
	    double_xpect[0].u = 0x4058c00000000000;
	    double_xpect[1].u = 0xc058c1ffffffffff;
	    double_xpect[2].u = 0x40c3860000000000;
	    double_xpect[3].u = 0xc0c387ffffffffff;
	    double_xpect[4].u = 0x4163120000000000;
	    double_xpect[5].u = 0xc16313ffffffffff;
	    double_xpect[6].u = 0x41cdcc0000000000;
	    double_xpect[7].u = 0xc1cdcdffffffffff;

	    for (x = 0; x < DIM_LEN_8; x++)
	    {
		fin.f = float_data_in[x];
		dfin.d = double_data_in[x];
		/* printf ("%d float_data_in : %08.8f   : 0x%x expected %08.8f   : 0x%x\n", */
		/* 	x, float_data_in[x], fin.u, xpect[x].f, xpect[x].u); */
		/* printf ("%d double_data_in : %15g   : 0x%lx expected %15g   : 0x%lx\n", */
		/* 	x, double_data_in[x], dfin.u, double_xpect[x].d, double_xpect[x].u); */
		if (fin.u != xpect[x].u)
		    ERR;
		if (dfin.u != double_xpect[x].u)
		    ERR;
	    }

            /* Close the file. */
            if (nc_close(ncid)) ERR;
        }
    }
    SUMMARIZE_ERR;
    FINAL_RESULTS;
}
