/* This is part of the netCDF package. Copyright 2018 University
   Corporation for Atmospheric Research/Unidata See COPYRIGHT file for
   conditions of use.

   Test HDF5 dataset code, even more. These are not intended to be
   exhaustive tests, but they use HDF5 the same way that netCDF-4
   does, so if these tests don't work, than netCDF-4 won't work
   either.
*/

#include "ccr_test.h"
#include <hdf5.h>
#include <H5DSpublic.h>
#include <netcdf.h>

#define FILE_NAME "tst_bzip2.nc"
#define STR_LEN 255
#define MAX_LEN 1024
#define H5Z_FILTER_BZIP2 307
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

size_t H5Z_filter_bzip2(unsigned int flags, size_t cd_nelmts,
                        const unsigned int cd_values[], size_t nbytes,
                        size_t *buf_size, void **buf);

int
main()
{
    printf("\n*** Checking HDF5 variable functions some more.\n");
    printf("*** Checking HDF5 variable compession and filters...");
    {
        int ncid;
        int dimid[NDIM2];
        int varid;
        /* hid_t fapl_id, fcpl_id; */
        /* hid_t datasetid; */
        /* hid_t fileid, grpid, spaceid, plistid; */
        int data_in[NX][NY], data_out[NX][NY];
        /* hsize_t fdims[NDIMS], fmaxdims[NDIMS]; */
        /* hsize_t chunksize[NDIMS], dimsize[NDIMS], maxdimsize[NDIMS]; */
        /* const unsigned cd_values[1] = {9};          /\* bzip2 default level is 9 *\/ */
        int x, y;
        const H5Z_class2_t H5Z_BZIP2[1] = {{
                H5Z_CLASS_T_VERS,       /* H5Z_class_t version */
                (H5Z_filter_t)H5Z_FILTER_BZIP2,         /* Filter id number             */
                1,              /* encoder_present flag (set to true) */
                1,              /* decoder_present flag (set to true) */
                "bzip2",                  /* Filter name for debugging    */
                NULL,                       /* The "can apply" callback     */
                NULL,                       /* The "set local" callback     */
                (H5Z_func_t)H5Z_filter_bzip2,         /* The actual filter function   */
            }};
        char plugin_path[MAX_LEN + 1];

        /* Create some data to write. */
        for (x = 0; x < NX; x++)
            for (y = 0; y < NY; y++)
                data_out[x][y] = x * NY + y;

        if (H5PLget(0, plugin_path, MAX_LEN) < 0) ERR;
        printf("plugin_path %s\n", plugin_path);

        if (H5Zregister(H5Z_BZIP2) < 0) ERR;

        if (!H5Zfilter_avail(H5Z_FILTER_BZIP2))
        {
            printf ("bzip2 filter not available.\n");
            return 1;
        }

        /* Create file. */
        if (nc_create(FILE_NAME, NC_NETCDF4, &ncid)) ERR;

        /* Create dims. */
        if (nc_def_dim(ncid, X_NAME, NX, &dimid[0]));
        if (nc_def_dim(ncid, Y_NAME, NY, &dimid[0]));

        /* /\* Set up compression. *\/ */
        /* if (H5Pset_filter (plistid, (H5Z_filter_t)307, H5Z_FLAG_MANDATORY, */
        /*                    (size_t)1, cd_values) < 0) ERR; */
        /* /\* if (H5Pset_deflate(plistid, DEFLATE_LEVEL) < 0) ERR; *\/ */

        /* Create the variable. */
        if (nc_def_var(ncid, VAR_NAME, NC_INT, NDIM2, dimid, &varid)) ERR;
        /* if ((datasetid = H5Dcreate2(grpid, SIMPLE_VAR_NAME, H5T_NATIVE_INT, */
        /*                             spaceid, H5P_DEFAULT, plistid, H5P_DEFAULT)) < 0) ERR; */

        /* /\* Write the data. *\/ */
        /* if (H5Dwrite(datasetid, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, */
        /*   	   H5P_DEFAULT, data_out) < 0) ERR; */

        if (nc_close(ncid)) ERR;

        /* Now reopen the file and check. */
        if (nc_open(FILE_NAME, NC_NETCDF4, &ncid)) ERR;
        if (nc_close(ncid)) ERR;
        /* if ((fapl_id = H5Pcreate(H5P_FILE_ACCESS)) < 0) ERR; */
        /* if (H5Pset_libver_bounds(fapl_id, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST) < 0) ERR; */
        /* if ((fileid = H5Fopen(FILE_NAME, H5F_ACC_RDONLY, fapl_id)) < 0) ERR; */
        /* if ((grpid = H5Gopen2(fileid, "/", H5P_DEFAULT)) < 0) ERR; */

        /* if ((datasetid = H5Dopen1(grpid, SIMPLE_VAR_NAME)) < 0) ERR; */
        /* if ((spaceid = H5Dget_space(datasetid)) < 0) */
        /* if (H5Sget_simple_extent_dims(spaceid, fdims, fmaxdims) > 0) ERR; */
        /* if (H5Dread(datasetid, H5T_NATIVE_INT, H5S_ALL, */
        /*   	  spaceid, H5P_DEFAULT, data_in) < 0) ERR; */

        /* /\* Check the data. *\/ */
        /* for (x = 0; x < NX; x++) */
        /*    for (y = 0; y < NY; y++) */
        /*       if (data_in[x][y] != data_out[x][y]) ERR; */

        /* if (H5Pclose(fapl_id) < 0 || */
        /*     H5Dclose(datasetid) < 0 || */
        /*     H5Sclose(spaceid) < 0 || */
        /*     H5Gclose(grpid) < 0 || */
        /*     H5Fclose(fileid) < 0) */
        /*    ERR; */
    }
    SUMMARIZE_ERR;
    FINAL_RESULTS;
}
