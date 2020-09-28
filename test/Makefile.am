# This file builds the test directory for the CCR library. These tests
# depend on netCDF.

# Ed Hartnett 12/30/2019

# Find the header files.
AM_CPPFLAGS = -I$(top_srcdir)/include

AM_LDFLAGS = ${top_builddir}/src/libccr.la
LDADD = ${top_builddir}/src/libccr.la

# Build the tests.
check_PROGRAMS = tst_bzip2

# Always run the bzip2 test.
TESTS = tst_bzip2

# Build LZ4 tests, if needed.
if BUILD_LZ4
TESTS += tst_lz4
check_PROGRAMS += tst_lz4
endif

# Build BitGroom tests, if needed.
if BUILD_BITGROOM
TESTS += tst_bitgroom
check_PROGRAMS += tst_bitgroom
endif

# Build Zstandard tests, if needed.
if BUILD_ZSTANDARD
TESTS += tst_zstandard
check_PROGRAMS += tst_zstandard
endif

# Delete files created in testing.
CLEANFILES = tst_*.nc
