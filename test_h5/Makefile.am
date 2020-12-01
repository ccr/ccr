# This file builds the test_h5 directory for the ccr library. This
# directory has tests that depend only on HDF5, not on netCDF.

# Ed Hartnett 12/24/2019

# Find the header files.
AM_CPPFLAGS = -I$(top_srcdir)/include

# Build the tests.
check_PROGRAMS = tst_h_bzip2
tst_h_bzip2_LDADD = ${top_builddir}/hdf5_plugins/BZIP2/src/libh5bz2.la

# Build the LZ4 tests?
if BUILD_LZ4
check_PROGRAMS += tst_h_lz4 tst_lz4_size
tst_h_lz4_LDADD = ${top_builddir}/hdf5_plugins/LZ4/src/libh5lz4.la
tst_lz4_size_LDADD = ${top_builddir}/hdf5_plugins/LZ4/src/libh5lz4.la
endif

# Build the BitGroom tests?
if BUILD_BITGROOM
check_PROGRAMS += tst_h_bitgroom
tst_h_bitgroom_LDADD = ${top_builddir}/hdf5_plugins/BITGROOM/src/libh5bgr.la
endif

# Build the Zstandard tests?
if BUILD_ZSTD
check_PROGRAMS += tst_h_zstandard tst_zstandard_size
tst_h_zstandard_LDFLAGS = ${top_builddir}/hdf5_plugins/ZSTANDARD/src/libh5zstd.la
tst_h_zstandard_LDADD = ${top_builddir}/hdf5_plugins/ZSTANDARD/src/libh5zstd.la
tst_zstandard_size_LDFLAGS = ${top_builddir}/hdf5_plugins/ZSTANDARD/src/libh5zstd.la
tst_zstandard_size_LDADD = ${top_builddir}/hdf5_plugins/ZSTANDARD/src/libh5zstd.la
endif

# Run the tests. This script is built by configure from
# run_tests.sh.in.
TESTS = run_tests.sh

# Delete files created in testing.
CLEANFILES = tst_*.h5

EXTRA_DIST = run_tests.sh.in

