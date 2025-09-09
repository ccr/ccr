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

# Does the user want LZ4?
AC_MSG_CHECKING([whether LZ4 filter library should be built and installed])
AC_ARG_ENABLE([lz4],
              [AS_HELP_STRING([--disable-lz4],
                              [Disable the build and install of lz4 filter library.])])
test "x$enable_lz4" = xno || enable_lz4=yes
AC_MSG_RESULT($enable_lz4)
AM_CONDITIONAL(BUILD_LZ4, [test "x$enable_lz4" = xyes])

if test "x$enable_lz4" = xyes; then
   AC_CONFIG_SUBDIRS([LZ4])
fi

# These files will be created when the configure script is run.
AC_CONFIG_FILES([Makefile
        ])
AC_OUTPUT()
