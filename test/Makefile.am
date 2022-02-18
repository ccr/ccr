# This file builds the test directory for the CCR library. These tests
# depend on netCDF.

# Ed Hartnett 12/30/2019

# Find the header files.
AM_CPPFLAGS = -I$(top_srcdir)/include

AM_LDFLAGS = ${top_builddir}/src/libccr.la
LDADD = ${top_builddir}/src/libccr.la
check_PROGRAMS =

# Build the bzip2 test, if needed.
if BUILD_BZIP2
check_PROGRAMS += tst_bzip2
endif

# Build LZ4 tests, if needed.
if BUILD_LZ4
check_PROGRAMS += tst_lz4
endif

# Build BitGroom tests, if needed.
if BUILD_BITGROOM
check_PROGRAMS += tst_bitgroom
endif

# Build Granular BitRound tests, if needed.
if BUILD_GRANULARBR
check_PROGRAMS += tst_granularbr
endif

# Build Zstandard tests, if needed.
if BUILD_ZSTD
check_PROGRAMS += tst_zstandard
endif

# Build the performance tests.
check_PROGRAMS += tst_perf tst_compress
tst_perf_SOURCES = tst_perf.c tst_utils.c
tst_compress_SOURCES = tst_compress.c tst_utils.c

# Run all the appropriate tests in this script.
TESTS = run_tests.sh

# Parallel I/O tests?
if BUILD_PARALLEL
check_PROGRAMS += tst_par
TESTS += run_par_tests.sh
endif

# Build benchmark program if desired.
if BUILD_BENCHMARKS
check_PROGRAMS += tst_eamv1_benchmark
tst_eamv1_benchmark_SOURCES = tst_eamv1_benchmark.c tst_utils.c
if BUILD_PARALLEL
check_PROGRAMS += tst_gfs_data_1
endif # BUILD_PARALLEL
TESTS += run_benchmarks.sh
endif

# Delete files created in testing.
CLEANFILES = tst_*.nc

EXTRA_DIST = run_tests.sh.in run_par_tests.sh.in run_benchmarks.sh.in

DISTCLEAN = run_tests.sh run_par_tests.sh run_benchmarks.sh
