# This is the main configure file for the hdf5_plugins project.

# Ed Hartnett 12/24/2019

AC_PREREQ([2.59])

# Initialize with name, version, and support email address.
AC_INIT([hdf5_plugins], [1.0], [])

AM_INIT_AUTOMAKE([foreign dist-zip])

# Keep libtool macros in an m4 directory.
AC_CONFIG_MACRO_DIR([m4])

# Set up libtool.
LT_PREREQ([2.4])
LT_INIT()

# If the env. variable HDF5_PLUGIN_DIR is set, or if
# --with-hdf5-plugin-dir=<directory>, use it as the plugin
# directory. This is necessary at the top level to provide the help
# message and --with option. If used, the option will be passed to the
# subdirs, and also handled by the configure in each filter
# subdirectory.
AC_MSG_CHECKING([where to put HDF5 plugins])
AC_ARG_WITH([hdf5-plugin-dir],
            [AS_HELP_STRING([--with-hdf5-plugin-dir=<directory>],
                            [specify HDF5 plugin directory (defaults to /usr/local/hdf5/lib/plugin, or value of HDF5_PLUGIN_DIR, if set)])],
            [HDF5_PLUGIN_DIR=$with_hdf5_plugin_dir])
HDF5_PLUGIN_DIR=${HDF5_PLUGIN_DIR-.}
AC_MSG_RESULT($HDF5_PLUGIN_DIR)

# Does the user want BZIP2?
AC_MSG_CHECKING([whether BZIP2 filter library should be built and installed])
AC_ARG_ENABLE([bzip2],
              [AS_HELP_STRING([--disable-bzip2],
                              [Disable the build and install of bzip2 filter library.])])
test "x$enable_bzip2" = xno || enable_bzip2=yes
AC_MSG_RESULT($enable_bzip2)
AM_CONDITIONAL(BUILD_BZIP2, [test "x$enable_bzip2" = xyes])

# Does the user want LZ4?
AC_MSG_CHECKING([whether LZ4 filter library should be built and installed])
AC_ARG_ENABLE([lz4],
              [AS_HELP_STRING([--disable-lz4],
                              [Disable the build and install of lz4 filter library.])])
test "x$enable_lz4" = xno || enable_lz4=yes
AC_MSG_RESULT($enable_lz4)
AM_CONDITIONAL(BUILD_LZ4, [test "x$enable_lz4" = xyes])

dnl # Does the user want BLOSC?
dnl AC_MSG_CHECKING([whether BLOSC filter library should be built and installed])
dnl AC_ARG_ENABLE([blosc],
dnl               [AS_HELP_STRING([--disable-blosc],
dnl                               [Disable the build and install of blosc filter library.])])
dnl test "x$enable_blosc" = xno || enable_blosc=yes
dnl AC_MSG_RESULT($enable_blosc)
AM_CONDITIONAL(BUILD_BLOSC, [test "x$enable_blosc" = xyes])

dnl # Does the user want JPEG?
dnl AC_MSG_CHECKING([whether JPEG filter library should be built and installed])
dnl AC_ARG_ENABLE([jpeg],
dnl               [AS_HELP_STRING([--disable-jpeg],
dnl                               [Disable the build and install of jpeg filter library.])])
dnl test "x$enable_jpeg" = xno || enable_jpeg=yes
dnl AC_MSG_RESULT($enable_jpeg)
AM_CONDITIONAL(BUILD_JPEG, [test "x$enable_jpeg" = xyes])

dnl # Does the user want LZF?
dnl AC_MSG_CHECKING([whether LZF filter library should be built and installed])
dnl AC_ARG_ENABLE([lzf],
dnl               [AS_HELP_STRING([--disable-lzf],
dnl                               [Disable the build and install of lzf filter library.])])
dnl test "x$enable_lzf" = xno || enable_lzf=yes
dnl AC_MSG_RESULT($enable_lzf)
AM_CONDITIONAL(BUILD_LZF, [test "x$enable_lzf" = xyes])

dnl # Does the user want BITGROOM?
dnl AC_MSG_CHECKING([whether BITGROOM filter library should be built and installed])
dnl AC_ARG_ENABLE([bitgroom],
dnl              [AS_HELP_STRING([--disable-bitgroom],
dnl                              [Disable the build and install of bitgroom filter library.])])
dnl test "x$enable_bitgroom" = xno || enable_bitgroom=yes
dnl AC_MSG_RESULT($enable_bitgroom)
AM_CONDITIONAL(BUILD_BITGROOM, [test "x$enable_bitgroom" = xyes])

# Build the filter libraries as desired.
if test "x$enable_bzip2" = xyes; then
   AC_CONFIG_SUBDIRS([BZIP2])
fi
if test "x$enable_lz4" = xyes; then
   AC_CONFIG_SUBDIRS([LZ4])
fi
dnl if test "x$enable_blosc" = xyes; then
dnl    AC_CONFIG_SUBDIRS([BLOSC])
dnl fi
dnl if test "x$enable_jpeg" = xyes; then
dnl    AC_CONFIG_SUBDIRS([JPEG])
dnl fi
dnl if test "x$enable_lzf" = xyes; then
dnl    AC_CONFIG_SUBDIRS([LZF])
dnl fi
dnl if test "x$enable_bitgroom" = xyes; then
dnl    AC_CONFIG_SUBDIRS([BITGROOM])
dnl fi
#AC_CONFIG_SUBDIRS([BSHUF])

# These files will be created when the configure script is run.
AC_CONFIG_FILES([Makefile
        ])
AC_OUTPUT()
