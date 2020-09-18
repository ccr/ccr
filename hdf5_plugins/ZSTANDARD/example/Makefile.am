# This builds the ZSTANDARD example directory

# Charlie Zender 9/15/20

# Link to our filter library
AM_LDFLAGS = ${top_builddir}/src/libh5zstd.la
LDADD = ${top_builddir}/src/libh5zstd.la

# Build example program and run it as a test
check_PROGRAMS = h5ex_d_zstandard
#TESTS = h5ex_d_zstandard

# Clean up HDF5 file created by example
CLEANFILES = *.h5
