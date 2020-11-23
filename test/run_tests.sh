# This script runs the tests in the CCR project.
#
# Ed Hartnett 11/18/20
set -x
set -e

# Set the plugin path to find plugin.
export HDF5_PLUGIN_PATH="../hdf5_plugins/BZIP2/src/.libs:$HDF5_PLUGIN_PATH"

# Run the example.
./tst_bzip2
