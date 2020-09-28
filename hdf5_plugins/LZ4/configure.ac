# Copyright by The HDF Group. All rights reserved.

# This is the main configure file for the LZ4 filter, a HDF5 plugin
# library that enables lz4 compression/decompression as a HDF5
# filter. This is part of the hdf5_plugin project.

# Allen Byrne, Ed Hartnett 1/14/19

# Initialize autoconf.
AC_PREREQ(2.59)
AC_INIT(H5LZ4, 1.0, help@hdfgroup.org)
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Initialize automake.
AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

LT_INIT(dlopen)

# If the env. variable HDF5_PLUGIN_PATH is set, or if
# --with-hdf5-plugin-dir=<directory>, use it as a place for the large
# (i.e. > 2 GiB) files created during the large file testing.
AC_MSG_CHECKING([where to put HDF5 plugins])
HDF5_PLUGIN_PATH=${HDF5_PLUGIN_PATH-'/usr/local/hdf5/lib/plugin'}
AC_ARG_WITH([hdf5-plugin-dir],
            [AS_HELP_STRING([--with-hdf5-plugin-dir=<directory>],
                            [specify HDF5 plugin directory (defaults to /usr/local/hdf5/lib/plugin, or value of HDF5_PLUGIN_PATH, if set)])],
            [HDF5_PLUGIN_PATH=$with_hdf5_plugin_path])
AC_MSG_RESULT($HDF5_PLUGIN_PATH)
AC_SUBST([HDF5_PLUGIN_PATH])

# Does the user want LZ4?
AC_MSG_CHECKING([whether LZ4 filter library should be built and installed])
AC_ARG_ENABLE([lz4],
              [AS_HELP_STRING([--disable-lz4],
                              [Disable the build and install of lz4 filter library.])])
test "x$enable_lz4" = xno || enable_lz4=yes
AC_MSG_RESULT($enable_lz4)
AM_CONDITIONAL(BUILD_LZ4, [test "x$enable_lz4" = xyes])

# Is the lz4 library present?
AC_CHECK_HEADERS([lz4.h], [], [AC_MSG_ERROR([lz4.h is required, set CPPFLAGS.])])
AC_CHECK_LIB([lz4], [LZ4_compress], [], [AC_MSG_ERROR([liblz4 is required, set LDFLAGS.])])

# We need the math library
AC_CHECK_LIB([m], [floor], [], [AC_MSG_ERROR([Math library is required.])])

# We need the HDF5 headers and library.
AC_CHECK_HEADERS([hdf5.h], [], [AC_MSG_ERROR([hdf5.h is required, set CPPFLAGS.])])
AC_SEARCH_LIBS([H5Fflush], [hdf5dll hdf5], [], [AC_MSG_ERROR([libhdf5 is required, set LDFLAGS.])])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MEMCMP
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memset])

# Check which plugins to build, if no environmental variables are set, build
# all.
if test ! "$PLUGIN_H5LZ4"
then
  PLUGIN_H5LZ4=1
fi
AM_CONDITIONAL(H5LZ4, test "$PLUGIN_H5LZ4")

## Set subdirectories
AC_CONFIG_FILES([Makefile
        example/Makefile
        src/Makefile])

# Output configure and all Makefile.in files.
AC_OUTPUT
