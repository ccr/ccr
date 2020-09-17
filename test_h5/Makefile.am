# This file builds the test_h5 directory for the ccr library. This
# directory has tests that depend only on HDF5, not on netCDF.

# Ed Hartnett 12/24/2019

# Find the header files.
AM_CPPFLAGS = -I$(top_srcdir)/include

# Build the tests.
check_PROGRAMS = tst_h_bzip2

# Each test must link to the filter it needs.
tst_h_bzip2_LDFLAGS = ${top_builddir}/hdf5_plugins/BZIP2/src/libh5bz2.la
tst_h_bzip2_LDADD = ${top_builddir}/hdf5_plugins/BZIP2/src/libh5bz2.la

# Run the tests.
TESTS = tst_h_bzip2

# Build the LZ4 tests?
if BUILD_LZ4
TESTS += tst_h_lz4 tst_lz4_size
check_PROGRAMS += tst_h_lz4 tst_lz4_size
tst_h_lz4_LDFLAGS = ${top_builddir}/hdf5_plugins/LZ4/src/libh5lz4.la
tst_h_lz4_LDADD = ${top_builddir}/hdf5_plugins/LZ4/src/libh5lz4.la
tst_lz4_size_LDFLAGS = ${top_builddir}/hdf5_plugins/LZ4/src/libh5lz4.la
tst_lz4_size_LDADD = ${top_builddir}/hdf5_plugins/LZ4/src/libh5lz4.la
endif

# Delete files created in testing.
CLEANFILES = tst_*.h5
