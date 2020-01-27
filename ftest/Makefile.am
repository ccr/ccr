# This file builds the ftest directory for the ccr library. These
# tests depend on netCDF.

# Ed Hartnett 1/23/2020

# Find the include files.
AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/fsrc

# Link to our assembled libraries.
AM_LDFLAGS = ${top_builddir}/fsrc/libccrf.la ${top_builddir}/src/libccr.la
LDADD = ${top_builddir}/fsrc/libccrf.la ${top_builddir}/src/libccr.la

# Run our test programs.
TESTS = ftst_ccr_bzip2 ftst_ccr_lz4

# Build our test programs.
check_PROGRAMS = ftst_ccr_bzip2 ftst_ccr_lz4

# Source code for the tests.
ftst_ccr_bzip2_SOURCES = ftst_ccr_bzip2.F90
ftst_ccr_lz4_SOURCES = ftst_ccr_lz4.F90

# Delete files created in testing.
CLEANFILES = ftst_*.nc
