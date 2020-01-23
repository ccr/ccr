# This file builds the ftest directory for the ccr library. These
# tests depend on netCDF.

# Ed Hartnett 1/23/2020

TEST_EXTENSIONS = .sh

# Find the include files.
AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/fsrc

# Link to our assembled libraries.
AM_LDFLAGS = ${top_builddir}/fsrc/libccrf.la ${top_builddir}/src/libccr.la
LDADD = ${top_builddir}/fsrc/libccrf.la ${top_builddir}/src/libccr.la

GLM_FTESTS = ftst_ccr_bzip2

# Build our test program.
check_PROGRAMS = ${GLM_FTESTS}

# Source code for the tests.
ftst_ccr_bzip2_SOURCES = ftst_ccr_bzip2.F90

# Run our test program.
TESTS = ${GLM_FTESTS}

# Make sure these files are included in distribution.
#EXTRA_DIST = CMakeLists.txt						\
#OR_GLM-L2-LCFA_G17_s20192692359400_e20192700000000_c20192700000028.nc

#CLEANFILES = ncdump2.nc

# Build the test.
# check_PROGRAMS = tst_bzip2 tst_lz4
# TESTS = tst_bzip2 tst_lz4

# Delete files created in testing.
CLEANFILES = tst_*.nc
