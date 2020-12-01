# This file builds the test directory for the CCR library. These tests
# depend on netCDF.

# Ed Hartnett 12/30/2019

# Find the header files.
AM_CPPFLAGS = -I$(top_srcdir)/include

AM_LDFLAGS = ${top_builddir}/src/libccr.la
LDADD = ${top_builddir}/src/libccr.la

# Build the bzip2 test.
check_PROGRAMS = tst_bzip2

# Build LZ4 tests, if needed.
if BUILD_LZ4
check_PROGRAMS += tst_lz4
endif

# Build BitGroom tests, if needed.
if BUILD_BITGROOM
check_PROGRAMS += tst_bitgroom
endif

# Build Zstandard tests, if needed.
if BUILD_ZSTD
check_PROGRAMS += tst_zstandard
endif

# Build the performance test.
check_PROGRAMS += tst_perf

# Run all the appropriate tests in this script.
TESTS = run_tests.sh

# Delete files created in testing.
CLEANFILES = tst_*.nc

EXTRA_DIST = run_tests.sh.in
