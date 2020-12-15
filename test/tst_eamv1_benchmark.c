/* This is part of the CCR package. Copyright 2020.

   Test performance by reading an existing climate data file, and
   rewriting some of the data in netCDF/HDF5 with filters. All 2D
   (time, ncol) and 3D (time, lve, ncol) float fields are copied.

   This requires a sample data file:
   20180129.DECKv1b_piControl.ne30_oEC.edison.cam.h0.0001-01.nc. 

   It is the January monthly mean output from the first year of the
   Pre-Industrial control simulation of EAMv1, the atmospheric
   component of E3SMv1 (i.e., analogous to CAM in CESM). That
   simulation is fully documented in:

   Golaz, J.-C., P. M. Caldwell, L. P. Van Roekel, M. R. Petersen,
   Q. Tang, J. D. Wolfe, and 75 co-authors (including CSZ) (2019), The
   DOE E3SM coupled model version 1: Overview and evaluation at
   standard resolution, J. Adv. Model. Earth Syst., 11(7), 2089-2129,
   https://doi.org/10.1029/2018MS001603.

   The dataset itself is also available through ESGF and through
   https://e3sm.org/data/get-e3sm-data/released-e3sm-data/v1-1-deg-data-cmip6/

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

#define NDIM2 2
#define NDIM3 3

#define MILLION 1000000
#define NFILE3 3
#define MAX_COMPRESSION_STR 4

#define LEV_SIZE 72
#define NCOL_SIZE 48602

#define NUM_2D_VAR 353
#define NUM_3D_VAR 65

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
    int ret;
    
    printf("\n*** Checking Performance of filters.\n");
#ifdef BUILD_ZSTD    
    printf("*** Checking Zstandard vs. zlib performance on large climate data set...");
    printf("\ncompression, level, read time (s), write time (s), re-read time (s), file size (MB)\n");
    {
        float *data_2d_in;
	int f;
	    
        if (!(data_2d_in = malloc(NCOL_SIZE * sizeof(float)))) ERR;
	
	/* Write three files, one uncompressed, one with zlib, and one with zstd. */
	for (f = 0; f < NFILE3; f++)
	{
	    int level = 1;
	    char compression[MAX_COMPRESSION_STR + 1];
	    char file_name[STR_LEN + 1];
	    int ncid_in;
	    int ncid;
	    int varid_2d_in[NUM_2D_VAR];
	    int varid_2d[NUM_2D_VAR];
	    int varid_3d_in[NUM_3D_VAR];
	    int varid_3d[NUM_3D_VAR];
	    int dimid_2d[NDIM2];
	    int dimid_3d[NDIM3];
	    char var_name_2d[NUM_2D_VAR][STR_LEN] = {"AEROD_v", "AODABS", "AODABSBC", "AODBC",
						  "AODDUST", "AODDUST1", "AODDUST3", "AODDUST4",
						  "AODMODE1", "AODMODE2", "AODMODE3", "AODMODE4",
						  "AODNIR", "AODPOM", "AODSO4", "AODSOA",
						  "AODSS", "AODUV", "AODVIS", "AQ_DMS",
						  "AQ_H2O2","AQ_H2SO4", "AQ_O3", "AQ_SO2",
						  "AQ_SOAG", "CDNUMC", "CLDHGH", "CLDHGH_CAL",
						  "CLDHGH_CAL_ICE", "CLDHGH_CAL_LIQ", "CLDHGH_CAL_UN", "CLDLOW",
						  "CLDLOW_CAL", "CLDLOW_CAL_ICE", "CLDLOW_CAL_LIQ", "CLDLOW_CAL_UN",
						  "CLDMED", "CLDMED_CAL", "CLDMED_CAL_ICE", "CLDMED_CAL_LIQ",
						  "CLDMED_CAL_UN", "CLDTOT", "CLDTOT_CAL", "CLDTOT_CAL_ICE",
						  "CLDTOT_CAL_LIQ", "CLDTOT_CAL_UN", "CLDTOT_ISCCP", "CLHMODIS",
						  "CLIMODIS", "CLLMODIS", "CLMMODIS", "CLWMODIS",
						  "DF_DMS", "DF_H2O2", "DF_H2SO4", "DF_O3",
						  "DF_SO2", "DF_SOAG", "DMS_SRF", "DP_KCLDBASE",
						  "DP_MFUP_MAX", "DP_WCLDBASE", "DSTSFMBL", "DTENDTH",
						  "DTENDTQ", "FLDS", "FLNS", "FLNSC",
						  "FLNT", "FLNTC", "FLUT", "FLUTC",
						  "FSDS", "FSDSC", "FSNS", "FSNSC",
						  "FSNT", "FSNTC", "FSNTOA", "FSNTOAC",
						  "FSUTOA", "FSUTOAC", "F_eff", "H2O2_SRF",
						  "H2SO4_SRF", "H2SO4_sfgaex1", "ICEFRAC", "IEFLX",
						  "IWPMODIS", "LANDFRAC", "LHFLX", "LINOZ_SZA",
						  "LND_MBL", "LWCF", "LWPMODIS", "MEANCLDALB_ISCCP",
						  "MEANPTOP_ISCCP", "MEANTAU_ISCCP", "MEANTBCLR_ISCCP", "MEANTB_ISCCP",
						  "O3_SRF", "OCNFRAC", "OMEGA500", "PBLH",
						  "PCTMODIS", "PHIS", "PRECC", "PRECL",
						  "PRECSC", "PRECSL", "PS", "PSL",
						  "QFLX", "RAM1", "REFFCLIMODIS", "REFFCLWMODIS",
						  "SFDMS", "SFH2O2", "SFH2SO4", "SFO3",
						  "SFSO2", "SFSOAG", "SFbc_a1", "SFbc_a3",
						  "SFbc_a4", "SFdst_a1", "SFdst_a3", "SFmom_a1",
						  "SFmom_a2", "SFmom_a3", "SFmom_a4", "SFncl_a1",
						  "SFncl_a2", "SFncl_a3", "SFnum_a1", "SFnum_a2",
						  "SFnum_a3", "SFnum_a4", "SFpom_a1", "SFpom_a3",
						  "SFpom_a4", "SFso4_a1", "SFso4_a2", "SFso4_a3",
						  "SFsoa_a1", "SFsoa_a2", "SFsoa_a3", "SHFLX",
						  "SH_KCLDBASE", "SH_MFUP_MAX", "SH_WCLDBASE", "SNOWHICE",
						  "SNOWHLND", "SO2_CLXF", "SO2_SRF", "SOAG_CLXF",
						  "SOAG_SRF", "SOAG_sfgaex1", "SOLIN", "SSAVIS",
						  "SSTSFMBL", "SSTSFMBL_OM", "SWCF", "TAUGWX",
						  "TAUGWY", "TAUILOGMODIS", "TAUIMODIS", "TAUTLOGMODIS",
						  "TAUTMODIS", "TAUWLOGMODIS", "TAUWMODIS", "TAUX",
						  "TAUY", "TGCLDCWP", "TGCLDIWP", "TGCLDLWP",
						  "TH7001000", "TMQ", "TREFHT", "TROP_P",
						  "TROP_T", "TS", "TSMN", "TSMX",
						  "TUH", "TUQ", "TVH", "TVQ",
						  "U10", "WD_H2O2", "WD_H2SO4", "WD_SO2",
						  "airFV", "bc_a1DDF", "bc_a1SFWET", "bc_a1_SRF",
						  "bc_a1_sfgaex1", "bc_a3DDF", "bc_a3SFWET", "bc_a3_SRF",
						  "bc_a4DDF", "bc_a4SFWET", "bc_a4_CLXF", "bc_a4_SRF",
						  "bc_a4_sfgaex1", "bc_c1DDF", "bc_c1SFWET", "bc_c3DDF",
						  "bc_c3SFWET", "bc_c4DDF", "bc_c4SFWET", "chla",
						  "dst_a1DDF", "dst_a1SF", "dst_a1SFWET", "dst_a1_SRF",
						  "dst_a3DDF", "dst_a3SF", "dst_a3SFWET", "dst_a3_SRF",
						  "dst_c1DDF", "dst_c1SFWET", "dst_c3DDF", "dst_c3SFWET",
						  "mlip", "mom_a1DDF", "mom_a1SF", "mom_a1SFWET",
						  "mom_a1_SRF", "mom_a1_sfgaex1", "mom_a2DDF", "mom_a2SF",
						  "mom_a2SFWET", "mom_a2_SRF", "mom_a3DDF", "mom_a3SFWET",
						  "mom_a3_SRF", "mom_a4DDF", "mom_a4SF", "mom_a4SFWET",
						  "mom_a4_SRF", "mom_a4_sfgaex1", "mom_c1DDF", "mom_c1SFWET",
						  "mom_c2DDF", "mom_c2SFWET", "mom_c3DDF", "mom_c3SFWET",
						  "mom_c4DDF", "mom_c4SFWET", "mpoly", "mprot",
						  "ncl_a1DDF", "ncl_a1SF", "ncl_a1SFWET", "ncl_a1_SRF",
						  "ncl_a2DDF", "ncl_a2SF", "ncl_a2SFWET", "ncl_a2_SRF",
						  "ncl_a3DDF", "ncl_a3SF", "ncl_a3SFWET", "ncl_a3_SRF",
						  "ncl_c1DDF", "ncl_c1SFWET", "ncl_c2DDF", "ncl_c2SFWET",
						  "ncl_c3DDF", "ncl_c3SFWET", "num_a1DDF", "num_a1SF",
						  "num_a1SFWET", "num_a1_CLXF", "num_a1_SRF", "num_a1_sfgaex1",
						  "num_a2DDF", "num_a2SFWET", "num_a2_CLXF", "num_a2_SRF",
						  "num_a3DDF", "num_a3SF", "num_a3SFWET", "num_a3_SRF",
						  "num_a4DDF", "num_a4SFWET", "num_a4_CLXF", "num_a4_SRF",
						  "num_a4_sfgaex1", "num_c1DDF", "num_c1SFWET", "num_c2DDF",
						  "num_c2SFWET", "num_c3DDF", "num_c3SFWET", "num_c4DDF",
						  "num_c4SFWET", "pom_a1DDF", "pom_a1SFWET", "pom_a1_SRF",
						  "pom_a1_sfgaex1", "pom_a3DDF", "pom_a3SFWET", "pom_a3_SRF",
						  "pom_a4DDF", "pom_a4SFWET", "pom_a4_CLXF", "pom_a4_SRF",
						  "pom_a4_sfgaex1", "pom_c1DDF", "pom_c1SFWET", "pom_c3DDF",
						  "pom_c3SFWET", "pom_c4DDF", "pom_c4SFWET", "so4_a1DDF",
						  "so4_a1SFWET", "so4_a1_CLXF", "so4_a1_SRF", "so4_a1_sfgaex1",
						  "so4_a2DDF", "so4_a2SFWET", "so4_a2_CLXF", "so4_a2_SRF",
						  "so4_a2_sfgaex1", "so4_a3DDF", "so4_a3SFWET", "so4_a3_SRF",
						  "so4_a3_sfgaex1", "so4_c1DDF", "so4_c1SFWET", "so4_c2DDF",
						  "so4_c2SFWET", "so4_c3DDF", "so4_c3SFWET", "soa_a1DDF",
						  "soa_a1SFWET", "soa_a1_SRF", "soa_a1_sfgaex1", "soa_a2DDF",
						  "soa_a2SFWET", "soa_a2_SRF", "soa_a2_sfgaex1", "soa_a3DDF",
						  "soa_a3SFWET", "soa_a3_SRF", "soa_a3_sfgaex1", "soa_c1DDF",
						  "soa_c1SFWET", "soa_c2DDF", "soa_c2SFWET", "soa_c3DDF",
						  "soa_c3SFWET" };
	    char var_name_3d[NUM_3D_VAR][STR_LEN] = {"ANRAIN", "ANSNOW", "AQRAIN", "AQSNOW",
						     "AREI", "AREL", "AWNC", "AWNI",
						     "CCN3", "CLDICE", "CLDLIQ", "CLOUD",
						     "CLOUDFRAC_CLUBB", "CONCLD", "DCQ", "DTCOND",
						     "EXTINCT", "FICE", "FREQI", "FREQL",
						     "FREQR", "FREQS", "ICIMR", "ICWMR",
						     "IWC", "LINOZ_DO3", "LINOZ_O3CLIM", "LINOZ_O3COL",
						     "LINOZ_SSO3", "Mass_bc", "Mass_dst", "Mass_mom",
						     "Mass_ncl", "Mass_pom", "Mass_so4", "Mass_soa",
						     "NUMICE", "NUMLIQ", "NUMRAI", "NUMSNO",
						     "O3", "OMEGA", "OMEGAT", "Q",
						     "QRL", "QRS", "RAINQM", "RELHUM",
						     "SNOWQM", "SO2", "T", "U",
						     "UU", "V", "VQ", "VT",
						     "VU", "VV", "WSUB", "Z3",
						     "aero_water", "extinct_lw_bnd7", "extinct_lw_inp", "extinct_sw_inp",
						     "hstobie_linoz"};
	    struct timeval start_time, end_time, diff_time;
	    int meta_write_us = 0;
	    int meta_read_us = 0;
	    int meta_reread_us = 0;
	    struct stat st;
	    int v;

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

	    /* Open input file. */
	    if (nc_open(INPUT_FILE, NC_NOWRITE, &ncid_in)) ERR;
	
	    /* Create output file. */
	    if (nc_create(file_name, NC_CLOBBER|NC_NETCDF4, &ncid)) ERR;
	    if (nc_def_dim(ncid, "time", NC_UNLIMITED, &dimid_2d[0])) ERR;
	    if (nc_def_dim(ncid, "ncol", NCOL_SIZE, &dimid_2d[1])) ERR;
	    dimid_3d[0] = dimid_2d[0];
	    if (nc_def_dim(ncid, "lev", LEV_SIZE, &dimid_3d[1])) ERR;
	    dimid_3d[2] = dimid_2d[1];

	    /* Define all 2D vars and set compression. */
	    for (v = 0; v < NUM_2D_VAR; v++)
	    {
		/* printf("v %d %s\n", v, var_name_2d[v]); */
		if (nc_def_var(ncid, var_name_2d[v], NC_FLOAT, NDIM2, dimid_2d, &varid_2d[v])) ERR;

		switch (f)
		{
		case 0:
		    /* no compression */
		    break;
		case 1:
		    if (nc_def_var_zstandard(ncid, varid_2d[v], level)) ERR;
		    break;
		case 2:
		    if (nc_def_var_deflate(ncid, varid_2d[v], 0, 1, level)) ERR;
		    break;
		}

		/* Get the varid for this var in the input file. */
		if (nc_inq_varid(ncid_in, var_name_2d[v], &varid_2d_in[v])) ERR;
	    } /* next var */

	    /* Define all 3D vars and set compression. */
	    for (v = 0; v < NUM_3D_VAR; v++)
	    {
		/* printf("v %d %s\n", v, var_name_3d[v]); */
		if ((ret = nc_def_var(ncid, var_name_3d[v], NC_FLOAT, NDIM3, dimid_3d, &varid_3d[v])))
		    NCERR(ret);

		switch (f)
		{
		case 0:
		    /* no compression */
		    break;
		case 1:
		    if (nc_def_var_zstandard(ncid, varid_3d[v], level)) ERR;
		    break;
		case 2:
		    if (nc_def_var_deflate(ncid, varid_3d[v], 0, 1, level)) ERR;
		    break;
		}

		/* Get the varid for this var in the input file. */
		if (nc_inq_varid(ncid_in, var_name_3d[v], &varid_3d_in[v])) ERR;
	    } /* next var */

	    /* Copy data from input file to output file. The first
	     * nc_put will turn off define mode in the file. */
	    for (v = 0; v < NUM_2D_VAR; v++)
	    {
		size_t start[NDIM2] = {0, 0};
		size_t count[NDIM2] = {1, NCOL_SIZE};
		
		/* printf("v %d %s\n", v, var_name_2d[v]); */

		/* Start timer. */
		if (gettimeofday(&start_time, NULL)) ERR;

		/* Read input data. */
		if (nc_get_var_float(ncid_in, varid_2d_in[v], data_2d_in)) ERR;

		/* Stop timer. */
		if (gettimeofday(&end_time, NULL)) ERR;
		if (nc4_timeval_subtract(&diff_time, &end_time, &start_time)) ERR;
		meta_read_us += (int)diff_time.tv_sec * MILLION + (int)diff_time.tv_usec;

		/* Start timer. */
		if (gettimeofday(&start_time, NULL)) ERR;

		/* Write data. */
		if (nc_put_vara_float(ncid, varid_2d[v], start, count, data_2d_in)) ERR;

		/* Stop timer. */
		if (gettimeofday(&end_time, NULL)) ERR;
		if (nc4_timeval_subtract(&diff_time, &end_time, &start_time)) ERR;
		meta_write_us += (int)diff_time.tv_sec * MILLION + (int)diff_time.tv_usec;
	    } /* next var */
	    
	    /* Copy 3D data from input file to output file. */
	    for (v = 0; v < NUM_3D_VAR; v++)
	    {
		size_t start[NDIM3] = {0, 0, 0};
		size_t count[NDIM3] = {1, 1, NCOL_SIZE};
		/* printf("v %d %s\n", v, var_name_3d[v]); */

		/* Read/write data for each level. */
		for (start[1] = 0; start[1] < LEV_SIZE; start[1]++)
		{
		    /* Start timer. */
		    if (gettimeofday(&start_time, NULL)) ERR;

		    /* Read input data. */
		    if (nc_get_vara_float(ncid_in, varid_3d_in[v], start, count, data_2d_in)) ERR;

		    /* Stop timer. */
		    if (gettimeofday(&end_time, NULL)) ERR;
		    if (nc4_timeval_subtract(&diff_time, &end_time, &start_time)) ERR;
		    meta_read_us += (int)diff_time.tv_sec * MILLION + (int)diff_time.tv_usec;

		    /* Start timer. */
		    if (gettimeofday(&start_time, NULL)) ERR;

		    /* Write data. */
		    if ((ret = nc_put_vara_float(ncid, varid_3d[v], start, count, data_2d_in)))
			NCERR(ret);

		    /* Stop timer. */
		    if (gettimeofday(&end_time, NULL)) ERR;
		    if (nc4_timeval_subtract(&diff_time, &end_time, &start_time)) ERR;
		    meta_write_us += (int)diff_time.tv_sec * MILLION + (int)diff_time.tv_usec;
		} /* next lev */
	    } /* next var */
	    
	    /* Start timer. Add the nc_close time to the write time,
	     * because buffers can be flushed in nc_close. */
	    if (gettimeofday(&start_time, NULL)) ERR;

	    /* Close output file. */
	    if (nc_close(ncid)) ERR;
	    
	    /* Stop timer. */
	    if (gettimeofday(&end_time, NULL)) ERR;
	    if (nc4_timeval_subtract(&diff_time, &end_time, &start_time)) ERR;
	    meta_write_us += (int)diff_time.tv_sec * MILLION + (int)diff_time.tv_usec;

	    /* Check the output file. */
	    {
		float *data_2d;

		if (!(data_2d = malloc(NCOL_SIZE * sizeof(float)))) ERR;
		
		/* Reopen the output file. */
		if (nc_open(file_name, NC_NOWRITE, &ncid)) ERR;
		
		/* Re-read data from input and output files 2D vars, to
		 * compare. Time how long it takes to re-read the data. */
		for (v = 0; v < NUM_2D_VAR; v++)
		{
		    int d;
		    
		    /* printf("v %d %s\n", v, var_name_2d[v]); */
		    
		    /* Read input data. */
		    if ((ret = nc_get_var_float(ncid_in, varid_2d_in[v], data_2d_in)))
			NCERR(ret);
		    
		    /* Start timer. */
		    if (gettimeofday(&start_time, NULL)) ERR;
		    
		    /* Read data from output file. */
		    if ((ret = nc_get_var_float(ncid, varid_2d[v], data_2d)))
			NCERR(ret);
		    
		    /* Stop timer. */
		    if (gettimeofday(&end_time, NULL)) ERR;
		    if (nc4_timeval_subtract(&diff_time, &end_time, &start_time)) ERR;
		    meta_reread_us += (int)diff_time.tv_sec * MILLION + (int)diff_time.tv_usec;

		    /* Check data values. */
		    for (d = 0; d < NCOL_SIZE; d++)
		    {
			/* if (data_2d_in[d] != data_2d[d]) ERR; */
		    }

		} /* next var */

		/* Re-read data from input and output files 3D vars, to
		 * compare. Time how long it takes to re-read the data. */
		for (v = 0; v < NUM_3D_VAR; v++)
		{
		    size_t start[NDIM3] = {0, 0, 0};
		    size_t count[NDIM3] = {1, 1, NCOL_SIZE};
		    int d;
		    int ret;
		    
		    /* printf("v %d %s\n", v, var_name_3d[v]); */
		    
		    /* Check data for each level. */
		    for (start[1] = 0; start[1] < LEV_SIZE; start[1]++)
		    {
			/* Read input data. */
			if ((ret = nc_get_vara_float(ncid_in, varid_3d_in[v], start, count, data_2d_in)))
			    NCERR(ret);
			
			/* Start timer. */
			if (gettimeofday(&start_time, NULL)) ERR;
			
			/* Read data from output file. */
			if ((ret = nc_get_vara_float(ncid, varid_3d[v], start, count, data_2d)))
			    NCERR(ret);
			
			/* Stop timer. */
			if (gettimeofday(&end_time, NULL)) ERR;
			if (nc4_timeval_subtract(&diff_time, &end_time, &start_time)) ERR;
			meta_reread_us += (int)diff_time.tv_sec * MILLION + (int)diff_time.tv_usec;
			
			/* Check data values. */
			for (d = 0; d < NCOL_SIZE; d++)
			{
			    /* if (data_2d_in[d] != data_2d[d]) ERR; */
			}
		    } /* next level */
		} /* next var */
		
		/* Close re-opened output file. */
		if (nc_close(ncid)) ERR;
		free(data_2d);
	    }
	    
	    /* Close input file. */
	    if (nc_close(ncid_in)) ERR;

	    stat(file_name, &st);
	    printf("%s, %d, %.3f, %.3f, %.3f, %.2f\n", (f ? compression : "none"), level, (float)meta_read_us/MILLION,
		   (float)meta_write_us/MILLION, (float)meta_reread_us/MILLION, (float)st.st_size/MILLION);

	} /* next file */
	free(data_2d_in);
    }
    SUMMARIZE_ERR;
#endif /* BUILD_ZSTD */
    FINAL_RESULTS;
}
