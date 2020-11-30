# Copyright by The HDF Group. All rights reserved.

# This builds the LZ4 example directory.

# Ed Hartnett 1/14/19

# Build example program and run it as a test.
if BUILD_LZ4
check_PROGRAMS = h5ex_d_lz4
TESTS = run_tests.sh
endif

# Clean up HDF5 file created by example.
CLEANFILES = *.h5

EXTRA_DIST = run_tests.sh
