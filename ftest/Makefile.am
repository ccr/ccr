# This file builds the ftest directory for the CCR library. These
# tests depend on netCDF.

# Ed Hartnett 1/23/2020, Charlie Zender 9/20/2020

# Find the include files.
AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/fsrc

# Link to our assembled libraries.
LDADD = ${top_builddir}/fsrc/libccrf.la ${top_builddir}/src/libccr.la -lnetcdff

# Build our test programs.
check_PROGRAMS = ftst_ccr_bzip2
ftst_ccr_bzip2_SOURCES = ftst_ccr_bzip2.F90

# Build the LZ4 tests?
if BUILD_LZ4
check_PROGRAMS += ftst_ccr_lz4
ftst_ccr_lz4_SOURCES = ftst_ccr_lz4.F90
endif

# Build the BitGroom tests?
if BUILD_BITGROOM
check_PROGRAMS += ftst_ccr_bitgroom
ftst_ccr_bitgroom_SOURCES = ftst_ccr_bitgroom.F90
endif

# Build the Granular BitRound tests?
if BUILD_GRANULARBR
check_PROGRAMS += ftst_ccr_granularbr
ftst_ccr_granularbr_SOURCES = ftst_ccr_granularbr.F90
endif

# Build the ZSTANDARD tests?
if BUILD_ZSTD
check_PROGRAMS += ftst_ccr_zstandard
ftst_ccr_zstandard_SOURCES = ftst_ccr_zstandard.F90
endif

# Run our test programs.
TESTS = run_tests.sh

# Delete files created in testing.
CLEANFILES = ftst_*.nc

# Distrubute this file.
EXTRA_DIST = run_tests.sh.in
