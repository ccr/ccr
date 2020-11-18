# This script runs the tests in the CCR project.
#
# Ed Hartnett 11/18/20

# Set the plugin path to find plugin.
export HDF5_PLUGIN_PATH="../src/.libs:$HDF5_PLUGIN_PATH"

# Run the example.
./tst_bzip2
