# This is the main configure file for the hdf5_plugins project.

# Ed Hartnett 12/24/2019

AC_PREREQ([2.59])

# Initialize with name, version, and support email address.
AC_INIT([hdf5_plugins], [1.0-development], [])

# Find out about the host we're building on.
AC_CANONICAL_HOST
AC_CANONICAL_BUILD

# Find out about the target we're building for.
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([foreign dist-zip subdir-objects])

# Keep libtool macros in an m4 directory.
AC_CONFIG_MACRO_DIR([m4])

# Set up libtool.
LT_PREREQ([2.4])
LT_INIT()

# Build the HDF5 plugins.
AC_CONFIG_SUBDIRS([BZIP2])
AC_CONFIG_SUBDIRS([LZ4])
AC_CONFIG_SUBDIRS([BITGROOM])

# These files will be created when the configure script is run.
AC_CONFIG_FILES([Makefile
        ])
AC_OUTPUT()
