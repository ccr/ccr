# This file builds the test directory for the ccr library. These tests
# depend on netCDF.

# Ed Hartnett 12/30/2019

# Find the header files.
AM_CPPFLAGS = -I$(top_srcdir)/include

AM_LDFLAGS = ${top_builddir}/src/libccr.la ${top_builddir}/hdf5_plugins/BZIP2/src/libh5bz2.la ${top_builddir}/hdf5_plugins/LZ4/src/libh5lz4.la
LDADD = ${top_builddir}/src/libccr.la ${top_builddir}/hdf5_plugins/BZIP2/src/libh5bz2.la ${top_builddir}/hdf5_plugins/LZ4/src/libh5lz4.la

# Build the test.
check_PROGRAMS = tst_bzip2
TESTS = tst_bzip2

# Delete files created in testing.
CLEANFILES = tst_*.nc
