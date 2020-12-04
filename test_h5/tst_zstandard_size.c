/*
 * Copyright by The HDF Group. All rights reserved.
 *
 * This is a test for the Zstandard filter.
 */

#include "config.h"
#include "ccr_test.h"
#include "hdf5.h"
#include <stdio.h>
#include <stdlib.h>

#define TEST "tst_zstandard_size"
#define DATASET "Spock"
#define DIM0 320
#define DIM1 640
#define CHUNK0 40
#define CHUNK1 80
#define H5Z_FILTER_ZSTANDARD 32015
#define NDIM2 2
#define NPARAM 1
#define NFILE 2
#define MAX_NAME 80

size_t H5Z_filter_zstandard(unsigned int flags, size_t cd_nelmts,
                      const unsigned int cd_values[], size_t nbytes,
                      size_t *buf_size, void **buf);

int
main (void)
{
    hid_t file_id = -1, space_id = -1, dset_id = -1, dcpl_id = -1;
    char filter_name[MAX_NAME + 1];
    hsize_t dims[NDIM2] = {DIM0, DIM1}, chunk[NDIM2] = {CHUNK0, CHUNK1};
    size_t nelmts = 1; /* number of elements in cd_values */
    unsigned int flags;
    unsigned filter_config;
    const unsigned int cd_values[NPARAM] = {3};     /* zstandard default is 3 */
    unsigned int values_out[NPARAM];
    int wdata[DIM0][DIM1], rdata[DIM0][DIM1];
    hsize_t f, i, j;
    const H5Z_class2_t H5Z_ZSTANDARD[1] = {{
            H5Z_CLASS_T_VERS,       /* H5Z_class_t version */
            (H5Z_filter_t)ZSTANDARD_ID,         /* Filter id number             */
            1,              /* encoder_present flag (set to true) */
            1,              /* decoder_present flag (set to true) */
            "Zstandard",                  /* Filter name for debugging    */
            NULL,                       /* The "can apply" callback     */
            NULL,                       /* The "set local" callback     */
            (H5Z_func_t)H5Z_filter_zstandard,         /* The actual filter function   */
        }};
    int ret_value = 1;

    /* Initialize data. */
    for (i = 0; i < DIM0; i++)
        for (j = 0; j < DIM1; j++)
            wdata[i][j] = i * j - j;

    printf("\n *** Testing ZSTANDARD compression filter...\n");

    if (H5Zregister(H5Z_ZSTANDARD) < 0) ERR;

    for (f = 0; f < NFILE; f++)
    {
        char filename[MAX_NAME + 1];

        sprintf(filename, "%s_%s.h5", TEST, (f ? "zstandard" : "uncompressed"));

        /* Create a new file using the default properties. */
        if ((file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT,
                                 H5P_DEFAULT)) < 0)
            goto done;

        /* Create dataspace. */
        if ((space_id = H5Screate_simple(2, dims, NULL)) < 0)
            goto done;

        /* Create the dataset creation property list. */
        if ((dcpl_id = H5Pcreate(H5P_DATASET_CREATE)) < 0)
            goto done;

        /* Add the ZSTANDARD compression filter. */
        if (f)
        {
            if (H5Pset_filter(dcpl_id, H5Z_FILTER_ZSTANDARD, H5Z_FLAG_MANDATORY, nelmts,
                              cd_values) < 0)
                goto done;

            /* Check that filter is registered with the library now. If it is
             * registered, retrieve filter's configuration. */
            if (H5Zfilter_avail(H5Z_FILTER_ZSTANDARD))
            {
                if (H5Zget_filter_info(H5Z_FILTER_ZSTANDARD, &filter_config) < 0)
                {
                    printf("failed to find filter.\n");
                    goto done;
                }
                if (!(filter_config & H5Z_FILTER_CONFIG_ENCODE_ENABLED) ||
                    !(filter_config & H5Z_FILTER_CONFIG_DECODE_ENABLED))
                {
                    printf("filter settings incorrect.\n");
                    goto done;
                }
            }
            else
            {
                printf("H5Zfilter_avail - not found.\n");
                goto done;
            }
        }

        if (H5Pset_chunk(dcpl_id, 2, chunk) < 0)
        {
            printf("failed to set chunk.\n");
            goto done;
        }

        /* Create the dataset. */
        if ((dset_id = H5Dcreate(file_id, DATASET, H5T_STD_I32LE, space_id,
                                 H5P_DEFAULT, dcpl_id, H5P_DEFAULT))  < 0)
        {
            printf("failed to create dataset.\n");
            goto done;
        }

        /* Write the data to the dataset. */
        if (H5Dwrite(dset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                     wdata[0]) < 0)
        {
            printf("failed to write data.\n");
            goto done;
        }

        /* Close and release resources. */
        H5Dclose(dset_id);
        dset_id = -1;
        H5Pclose(dcpl_id);
        dcpl_id = -1;
        H5Sclose(space_id);
        space_id = -1;
        H5Fclose(file_id);
        file_id = -1;

        /* Open file and dataset using the default properties. */
        if ((file_id = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT))  < 0)
            goto done;
        if ((dset_id = H5Dopen(file_id, DATASET, H5P_DEFAULT))  < 0)
            goto done;

        /* Retrieve dataset creation property list. */
        if ((dcpl_id = H5Dget_create_plist(dset_id)) < 0)
            goto done;

        /* Retrieve and print the filter id, compression level and
         * filter's name for Zstandard. */
        if (f)
        {
            if (H5Pget_filter2(dcpl_id, (unsigned) 0, &flags, &nelmts, values_out,
                               sizeof(filter_name), filter_name, NULL) != H5Z_FILTER_ZSTANDARD)
            {
                printf ("Not expected filter\n");
                goto done;
            }
        }

        /* Read the data using the default properties.  */
        if (H5Dread(dset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                    rdata[0]) < 0)
        {
            printf ("failed to read data.\n");
            goto done;
        }

        /* Check the data. */
        for (i = 0; i < DIM0; i++)
        {
            for (j = 0; j < DIM1; j++)
            {
                if (rdata[i][j] != wdata[i][j])
                {
                    printf("failed to read correct data.\n");
                    goto done;
                }
            }
        }

        /* Check again that filter is registered with the library. */
        if (!H5Zfilter_avail(H5Z_FILTER_ZSTANDARD))
        {
            printf ("failed to find filter.\n");
            goto done;
        }
    } /* next file */

    printf (" *** SUCCESS!!! ***\n");
    ret_value = 0;

done:
    /* Close and release resources. */
    if (dcpl_id >= 0) H5Pclose (dcpl_id);
    if (dset_id >= 0) H5Dclose (dset_id);
    if (space_id >= 0) H5Sclose (space_id);
    if (file_id >= 0) H5Fclose (file_id);

    SUMMARIZE_ERR;
    FINAL_RESULTS;

    return ret_value;
}
