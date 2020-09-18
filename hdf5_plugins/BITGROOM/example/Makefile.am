# This builds the BitGroom example directory

# Charlie Zender 9/12/20

# Link to our filter library
AM_LDFLAGS = ${top_builddir}/src/libh5bgr.la
LDADD = ${top_builddir}/src/libh5bgr.la

# Build example program and run it as a test
check_PROGRAMS = h5ex_d_bitgroom
#TESTS = h5ex_d_bitgroom

# Clean up HDF5 file created by example.
CLEANFILES = *.h5
