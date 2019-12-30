/* This is the code file for the Community Codex Repository library for netCDF.
 *
 * Ed Hartnett
 * 12/30/19
 */

#include "ccr.h"
#include <hdf5.h>
#include <H5DSpublic.h>

#define H5Z_FILTER_BZIP2 307

size_t H5Z_filter_bzip2(unsigned int flags, size_t cd_nelmts,
                        const unsigned int cd_values[], size_t nbytes,
                        size_t *buf_size, void **buf);

int ccr_init()
{
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
    /* char plugin_path[MAX_LEN + 1]; */
    /* if (H5PLget(0, plugin_path, MAX_LEN) < 0) ERR; */
    /* printf("plugin_path %s\n", plugin_path); */

    if (H5Zregister(H5Z_BZIP2) < 0)
        return 2;

    if (!H5Zfilter_avail(H5Z_FILTER_BZIP2))
    {
        printf ("bzip2 filter not available.\n");
        return 1;
    }
    return 0;
}
