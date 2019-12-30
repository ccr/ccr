# This file builds the test_h5 directory for the ccr library. This
# directory has tests that depend only on HDF5, not on netCDF.

# Ed Hartnett 12/24/2019

# Find the header files.
AM_CPPFLAGS = -I$(top_srcdir)/include

# Build the tests.
check_PROGRAMS = tst_h_bzip2 tst_h_lz4

# Each test must link to the filter it needs.
tst_h_bzip2_AM_LDFLAGS = ${top_builddir}/hdf5_plugins/BZIP2/src/libh5bz2.la
tst_h_bzip2_LDADD = ${top_builddir}/hdf5_plugins/BZIP2/src/libh5bz2.la
tst_h_lz4_AM_LDFLAGS = ${top_builddir}/hdf5_plugins/BZIP2/src/libh5bz2.la
tst_h_lz4_LDADD = ${top_builddir}/hdf5_plugins/BZIP2/src/libh5bz2.la

# Run the tests.
TESTS = tst_h_bzip2 tst_h_lz4

# Delete files created in testing.
CLEANFILES = tst_*.h5
