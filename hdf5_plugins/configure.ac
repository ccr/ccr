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

# If the env. variable HDF5_PLUGIN_PATH is set, or if
# --with-hdf5-plugin-path=<directory>, use it as the plugin
# directory. This is necessary at the top level to provide the help
# message and --with option. If used, the option will be passed to the
# subdirs, and also handled by the configure in each filter
# subdirectory.
AC_MSG_CHECKING([where to put HDF5 plugins])
AC_ARG_WITH([hdf5-plugin-path],
            [AS_HELP_STRING([--with-hdf5-plugin-path=<directory>],
                            [specify HDF5 plugin path (defaults to /usr/local/hdf5/lib/plugin, or value of HDF5_PLUGIN_PATH, if set)])],
            [HDF5_PLUGIN_PATH=$with_hdf5_plugin_path])
HDF5_PLUGIN_PATH=${HDF5_PLUGIN_PATH-.}
AC_MSG_RESULT($HDF5_PLUGIN_PATH)

# Does the user want BZIP2?
AC_MSG_CHECKING([whether BZIP2 filter library should be built and installed])
AC_ARG_ENABLE([bzip2],
              [AS_HELP_STRING([--disable-bzip2],
                              [Disable the build and install of bzip2 filter library.])])
test "x$enable_bzip2" = xno || enable_bzip2=yes
AC_MSG_RESULT($enable_bzip2)
AM_CONDITIONAL(BUILD_BZIP2, [test "x$enable_bzip2" = xyes])

# Does the user want LZ4? Too bad, they can't have it yet! See
# https://github.com/ccr/ccr/issues/20.
dnl AC_MSG_CHECKING([whether LZ4 filter library should be built and installed])
dnl AC_ARG_ENABLE([lz4],
dnl               [AS_HELP_STRING([--disable-lz4],
dnl                               [Disable the build and install of lz4 filter library.])])
dnl test "x$enable_lz4" = xno || enable_lz4=yes
enable_lz4=no
dnl AC_MSG_RESULT($enable_lz4)
AM_CONDITIONAL(BUILD_LZ4, [test "x$enable_lz4" = xyes])

# Does the user want BitGroom?
AC_MSG_CHECKING([whether BitGroom filter library should be built and installed])
AC_ARG_ENABLE([bitgroom],
              [AS_HELP_STRING([--disable-bitgroom],
                              [Disable the build and install of BitGroom filter library.])])
test "x$enable_bitgroom" = xno || enable_bitgroom=yes
AC_MSG_RESULT($enable_bitgroom)
AM_CONDITIONAL(BUILD_BITGROOM, [test "x$enable_bitgroom" = xyes])

# Does the user want Granular BitRound?
AC_MSG_CHECKING([whether Granular BitRound filter library should be built and installed])
AC_ARG_ENABLE([granularbr],
              [AS_HELP_STRING([--disable-granularbr],
                              [Disable the build and install of Granular BitRound filter library.])])
test "x$enable_granularbr" = xno || enable_granularbr=yes
AC_MSG_RESULT($enable_granularbr)
AM_CONDITIONAL(BUILD_GRANULARBR, [test "x$enable_granularbr" = xyes])

# Does the user want Zstandard?
AC_MSG_CHECKING([whether Zstandard filter library should be built and installed])
AC_ARG_ENABLE([zstd],
              [AS_HELP_STRING([--disable-zstd],
                              [Disable the build and install of Zstandard filter library.])])
test "x$enable_zstd" = xno || enable_zstd=yes
AC_MSG_RESULT($enable_zstd)
AM_CONDITIONAL(BUILD_ZSTANDARD, [test "x$enable_zstd" = xyes])

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

# Build the filter libraries as desired.
if test "x$enable_bzip2" = xyes; then
   AC_CONFIG_SUBDIRS([BZIP2])
fi
dnl if test "x$enable_lz4" = xyes; then
dnl    AC_CONFIG_SUBDIRS([LZ4])
dnl fi
if test "x$enable_bitgroom" = xyes; then
   AC_CONFIG_SUBDIRS([BITGROOM])
fi
if test "x$enable_granularbr" = xyes; then
   AC_CONFIG_SUBDIRS([GRANULARBR])
fi
if test "x$enable_zstd" = xyes; then
   AC_CONFIG_SUBDIRS([ZSTANDARD])
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
#AC_CONFIG_SUBDIRS([BSHUF])

# These files will be created when the configure script is run.
AC_CONFIG_FILES([Makefile
        ])
AC_OUTPUT()
