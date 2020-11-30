# This is the main configure file for the netCDF Community Codec
# Repository (ccr).

# Ed Hartnett 12/24/2019

AC_PREREQ([2.59])

# Initialize with name, version, and support email address.
AC_INIT([ccr], [1.1-development], [])

AM_INIT_AUTOMAKE([foreign dist-zip subdir-objects])

# Keep libtool macros in an m4 directory.
AC_CONFIG_MACRO_DIR([m4])

# Set up libtool.
LT_PREREQ([2.4])
LT_INIT()

# Find the C compiler.
AC_PROG_CC
AM_PROG_CC_C_O
AC_C_CONST
AC_PROG_CPP

# Find the Fortran compiler.
AC_PROG_FC
AC_PROG_F77

# Find the install program.
AC_PROG_INSTALL

# Check to see if any macros must be set to enable large (>2GB) files.
AC_SYS_LARGEFILE

if test "x$enable_shared" = xno; then
   AC_MSG_ERROR([Only shared library builds allowed.])
fi

# Does the user want to enable Fortran library?
AC_MSG_CHECKING([whether Fortran library should be built])
AC_ARG_ENABLE([fortran],
              [AS_HELP_STRING([--enable-fortran],
                              [build the CCR Fortran library.])])
test "x$enable_fortran" = xyes || enable_fortran=no
AC_MSG_RESULT([$enable_fortran])
AM_CONDITIONAL(BUILD_FORTRAN, [test "x$enable_fortran" = xyes])

# Does the user want to build documentation?
AC_MSG_CHECKING([whether documentation should be build (requires doxygen)])
AC_ARG_ENABLE([docs],
              [AS_HELP_STRING([--enable-docs],
                              [enable building of documentation with doxygen.])])
test "x$enable_docs" = xyes || enable_docs=no
AC_MSG_RESULT([$enable_docs])
AM_CONDITIONAL(BUILD_DOCS, [test "x$enable_docs" = xyes])

# Is doxygen installed?
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN" -a "x$enable_docs" = xyes; then
   AC_MSG_ERROR([Doxygen not found but --enable-docs used.])
fi

# If building docs, process Doxyfile.in into Doxyfile.
if test "x$enable_docs" = xyes; then
   AC_CONFIG_FILES([docs/Doxyfile])
fi

# If the env. variable HDF5_PLUGIN_PATH is set, or if
# --with-hdf5-plugin-dir=<directory>, use it as the plugin
# directory. This is necessary at the top level to provide the help
# message and --with option. If used, the option will be passed to the
# subdirs, and also handled by the configure in each filter
# subdirectory.
AC_MSG_CHECKING([where to put HDF5 plugins])
AC_ARG_WITH([hdf5-plugin-dir],
            [AS_HELP_STRING([--with-hdf5-plugin-dir=<directory>],
                            [specify HDF5 plugin directory (defaults to /usr/local/hdf5/lib/plugin, or value of HDF5_PLUGIN_PATH, if set)])],
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
AC_SUBST([BUILD_BZIP2], [$enable_bzip2])

# Does the user want LZ4?
AC_MSG_CHECKING([whether LZ4 filter library should be built and installed])
AC_ARG_ENABLE([lz4],
              [AS_HELP_STRING([--disable-lz4],
                              [Disable the build and install of lz4 filter library.])])
test "x$enable_lz4" = xno || enable_lz4=yes
AC_MSG_RESULT($enable_lz4)
AM_CONDITIONAL(BUILD_LZ4, [test "x$enable_lz4" = xyes])
if test "x$enable_lz4" = xyes; then
   AC_DEFINE([BUILD_LZ4], 1, [If true, build with lz4 filter.])
fi
AC_SUBST([BUILD_LZ4], [$enable_lz4])

# Does the user want BITGROOM?
AC_MSG_CHECKING([whether BITGROOM filter library should be built and installed])
AC_ARG_ENABLE([bitgroom],
              [AS_HELP_STRING([--disable-bitgroom],
                              [Disable the build and install of BitGroom filter library.])])
test "x$enable_bitgroom" = xno || enable_bitgroom=yes
AC_MSG_RESULT($enable_bitgroom)
AM_CONDITIONAL(BUILD_BITGROOM, [test "x$enable_bitgroom" = xyes])
AC_SUBST([BUILD_BITGROOM], [$enable_bitgroom])

# Does the user want Zstandard?
AC_MSG_CHECKING([whether Zstandard filter library should be built and installed])
AC_ARG_ENABLE([zstd],
              [AS_HELP_STRING([--disable-zstd],
                              [Disable the build and install of Zstandard filter library.])])
test "x$enable_zstd" = xno || enable_zstd=yes
AC_MSG_RESULT($enable_zstd)
AM_CONDITIONAL(BUILD_ZSTD, [test "x$enable_zstd" = xyes])
AC_SUBST([BUILD_ZSTD], [$enable_zstd])

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

# We need the math library
AC_CHECK_LIB([m], [floor], [],
[AC_MSG_ERROR([Can't find or link to the math library.])])

# Check for netCDF C library.
AC_SEARCH_LIBS([nc_create], [netcdf], [],
                            [AC_MSG_ERROR([Can't find or link to the netCDF C library, set CPPFLAGS/LDFLAGS.])])
AC_CHECK_HEADERS([netcdf.h netcdf_meta.h])

# Do we have netCDF-4?
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include "netcdf_meta.h"],
[[#if !NC_HAS_NC4
# error
#endif]
])], [have_netcdf4=yes], [have_netcdf4=no])
AC_MSG_CHECKING([whether netCDF provides netCDF/HDF5])
AC_MSG_RESULT([${have_netcdf4}])

# Do we have a parallel build of netCDF-4? (Really we should be
# checking NC_HAS_PARALLEL4, but that was only recently introduced, so
# we will go with NC_HAS_PARALLEL.)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include "netcdf_meta.h"],
[[#if !NC_HAS_PARALLEL
# error
#endif]
])], [have_netcdf_par=yes], [have_netcdf_par=no])
AC_MSG_CHECKING([whether netCDF provides parallel I/O for netCDF/HDF5])
AC_MSG_RESULT([${have_netcdf_par}])

# Do we have szip?
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include "netcdf_meta.h"],
[[#if !NC_HAS_SZIP_WRITE
# error
#endif]
])], [have_szip_write=yes], [have_szip_write=no])
AC_MSG_CHECKING([whether netCDF provides szip write capability])
AC_MSG_RESULT([${have_szip_write}])

# Do we have parallel filter support? Parallel filters are required
# for iotype NETCDF4P to use compression.
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include "netcdf_meta.h"],
[[#if !NC_HAS_PAR_FILTERS
# error
#endif]
])], [have_par_filters=yes], [have_par_filters=no])
AC_MSG_CHECKING([whether netCDF provides parallel filter support])
AC_MSG_RESULT([${have_par_filters}])
if test "x$have_par_filters" = xyes ; then
   AC_DEFINE([HAVE_PAR_FILTERS], [1], [if true, netCDF-C supports filters with parallel I/O])
fi

# Is this version 4.7.2, which does not work?
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include "netcdf_meta.h"],
[[#if NC_VERSION_MAJOR == 4 && NC_VERSION_MINOR == 7 && NC_VERSION_PATCH == 2
#else
# error
#endif]
])], [have_472=yes], [have_472=no])
AC_MSG_CHECKING([whether this is netcdf-c-4.7.2])
AC_MSG_RESULT([${have_472}])
if test "x$have_472" = xyes; then
   AC_MSG_ERROR([cannot build with netcdf-c-4.7.2, please upgrade your netCDF version.])
fi

# Check for netCDF Fortran library.
if test "x$enable_fortran" = xyes; then
   AC_LANG_PUSH(Fortran)
   AC_SEARCH_LIBS([nf_create], [netcdff], [], [AC_MSG_ERROR([Can't find or link to the netCDF Fortran library, set CPPFLAGS/LDFLAGS.])])
   AC_LANG_POP(Fortran)
fi

# Check for the main hdf5 and hdf5_hl library.
AC_SEARCH_LIBS([H5Fflush], [hdf5dll hdf5], [], [AC_MSG_ERROR([Can't find or link to the HDF5 library.])])
AC_SEARCH_LIBS([H5DSis_scale], [hdf5_hldll hdf5_hl], [],
                                  [AC_MSG_ERROR([Can't find or link to the HDF5 high-level library.])])

# Find the dynamic load library.
AC_SEARCH_LIBS([dlopen], [dl dld], [], [])

# Configure the test running script.
AC_CONFIG_FILES([test/run_tests.sh], [chmod ugo+x test/run_tests.sh])
AC_CONFIG_FILES([ftest/run_tests.sh], [chmod ugo+x ftest/run_tests.sh])
AC_CONFIG_FILES([test_h5/run_tests.sh], [chmod ugo+x test_h5/run_tests.sh])

# Build the HDF5 plugins.
AC_CONFIG_SUBDIRS([hdf5_plugins])

# Generate the config.h file.
AC_CONFIG_HEADERS([config.h])

# These files will be created when the configure script is run.
AC_CONFIG_FILES([Makefile
	src/Makefile
        fsrc/Makefile
	include/Makefile
	test_h5/Makefile
	ftest/Makefile
	test/Makefile
	docs/Makefile
        ])
AC_OUTPUT()
