# This file builds the test directory for the CCR library. These tests
# depend on netCDF.

# Ed Hartnett 12/30/2019

# Find the header files.
AM_CPPFLAGS = -I$(top_srcdir)/include

AM_LDFLAGS = ${top_builddir}/src/libccr.la ${top_builddir}/hdf5_plugins/BZIP2/src/libh5bz2.la

# Build the tests.
check_PROGRAMS = tst_bzip2 tst_lz4 tst_bitgroom tst_zstandard

# Always run the bzip2 test.
TESTS = tst_bzip2
tst_bzip2_LDADD = ${top_builddir}/src/libccr.la ${top_builddir}/hdf5_plugins/BZIP2/src/libh5bz2.la

# Build LZ4 tests, if needed.
if BUILD_LZ4
AM_LDFLAGS += ${top_builddir}/hdf5_plugins/LZ4/src/libh5lz4.la
TESTS += tst_lz4
tst_lz4_LDADD = ${top_builddir}/src/libccr.la ${top_builddir}/hdf5_plugins/LZ4/src/libh5lz4.la
endif

# Build BitGroom tests, if needed.
if BUILD_BITGROOM
AM_LDFLAGS += ${top_builddir}/hdf5_plugins/BITGROOM/src/libh5bgr.la
TESTS += tst_bitgroom
tst_bitgroom_LDADD = ${top_builddir}/hdf5_plugins/BITGROOM/src/libh5bgr.la
endif

# Build Zstandard tests, if needed.
if BUILD_ZSTANDARD
AM_LDFLAGS += ${top_builddir}/hdf5_plugins/ZSTANDARD/src/libh5zstd.la
TESTS += tst_zstandard
tst_zstandard_LDADD = ${top_builddir}/hdf5_plugins/ZSTANDARD/src/libh5zstd.la
endif

# Delete files created in testing.
CLEANFILES = tst_*.nc
