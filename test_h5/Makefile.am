# This file builds the test_h5 directory for the ccr library. This
# directory has tests that depend only on HDF5, not on netCDF.

# Ed Hartnett 12/24/2019

# Find the header files.
AM_CPPFLAGS = -I$(top_srcdir)/include

AM_LDFLAGS = ${top_builddir}/hdf5_plugins/BZIP2/src/libh5bz2.la
LDADD = ${top_builddir}/hdf5_plugins/BZIP2/src/libh5bz2.la

# Build the test.
check_PROGRAMS = tst_h_bzip2
TESTS = tst_h_bzip2

# Delete files created in testing.
CLEANFILES = tst_*.h5
