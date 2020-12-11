# This is the main configure file for the netCDF Community Codec
# Repository (ccr).

# Ed Hartnett 12/24/2019

AC_PREREQ([2.59])

# Initialize with name, version, and support email address.
AC_INIT([ccr], [1.0.0-development], [])

AM_INIT_AUTOMAKE([foreign dist-zip subdir-objects])

# Keep libtool macros in an m4 directory.
AC_CONFIG_MACRO_DIR([m4])

# Set up libtool.
LT_PREREQ([2.4])
LT_INIT()

# Components of the version.
AC_SUBST([CCR_VERSION_MAJOR]) CCR_VERSION_MAJOR=1
AC_SUBST([CCR_VERSION_MINOR]) CCR_VERSION_MINOR=0
AC_SUBST([CCR_VERSION_PATCH]) CCR_VERSION_PATCH=0
AC_SUBST([CCR_VERSION_NOTE]) CCR_VERSION_NOTE="-development"
AC_SUBST([CCR_VERSION]) CCR_VERSION=$VERSION

# Configuration Date
if test "x$SOURCE_DATE_EPOCH" != "x" ; then
    AC_SUBST([CONFIG_DATE]) CONFIG_DATE="`date -u -d "${SOURCE_DATE_EPOCH}"`"
else
    AC_SUBST([CONFIG_DATE]) CONFIG_DATE="`date`"
fi

# Prefer an empty CFLAGS variable instead of the default -g -O2. See:
# http://www.gnu.org/savannah-checkouts/gnu/autoconf/manual/autoconf-2.69/html_node/C-Compiler.html#C-Compiler
: ${CFLAGS=""}

## Compiler with version information. This consists of the full path
## name of the compiler and the reported version number.
AC_SUBST([CC_VERSION])
## Strip anything that looks like a flag off of $CC
CC_NOFLAGS=`echo $CC | sed 's/ -.*//'`

if `echo $CC_NOFLAGS | grep ^/ >/dev/null 2>&1`; then
  CC_VERSION="$CC"
else
  CC_VERSION="$CC";
  for x in `echo $PATH | sed -e 's/:/ /g'`; do
    if test -x $x/$CC_NOFLAGS; then
      CC_VERSION="$x/$CC"
      break
    fi
  done
fi
if test -n "$cc_version_info"; then
  CC_VERSION="$CC_VERSION ( $cc_version_info)"
fi

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
if test "x$enable_bzip2" = xyes; then
   AC_DEFINE([BUILD_BZIP], 1, [If true, build with bzip filter.])
fi
AC_SUBST([BUILD_BZIP2], [$enable_bzip2])

# Does the user want LZ4? Too bad, they can't have it yet! See
# https://github.com/ccr/ccr/issues/20.
dnl AC_MSG_CHECKING([whether LZ4 filter library should be built and installed])
dnl AC_ARG_ENABLE([lz4],
dnl               [AS_HELP_STRING([--disable-lz4],
dnl                               [Disable the build and install of lz4 filter library.])])
dnl test "x$enable_lz4" = xno || enable_lz4=yes
dnl AC_MSG_RESULT($enable_lz4)
enable_lz4=no
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
if test "x$enable_bitgroom" = xyes; then
   AC_DEFINE([BUILD_BITGROOM], 1, [If true, build with bitgroom filter.])
fi
AC_SUBST([BUILD_BITGROOM], [$enable_bitgroom])

# Does the user want Zstandard?
AC_MSG_CHECKING([whether Zstandard filter library should be built and installed])
AC_ARG_ENABLE([zstd],
              [AS_HELP_STRING([--disable-zstd],
                              [Disable the build and install of Zstandard filter library.])])
test "x$enable_zstd" = xno || enable_zstd=yes
AC_MSG_RESULT($enable_zstd)
AM_CONDITIONAL(BUILD_ZSTD, [test "x$enable_zstd" = xyes])
if test "x$enable_zstd" = xyes; then
   AC_DEFINE([BUILD_ZSTD], 1, [If true, build with zstd filter.])
fi
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

# Does the user want to run extra parallel tests when parallel netCDF-4 is built?
AC_MSG_CHECKING([whether parallel IO tests should be run])
AC_ARG_ENABLE([parallel-tests],
              [AS_HELP_STRING([--enable-parallel-tests],
                              [Run extra parallel IO tests. Requires netCDF
                              with parallel I/O support.])])
test "x$enable_parallel_tests" = xyes || enable_parallel_tests=no
AC_MSG_RESULT($enable_parallel_tests)
AM_CONDITIONAL(BUILD_PARALLEL, [test "x$enable_parallel_tests" = xyes])

# Did the user specify an MPI launcher other than mpiexec?
AC_MSG_CHECKING([whether a user specified program to run mpi programs])
AC_ARG_WITH([mpiexec],
              [AS_HELP_STRING([--with-mpiexec=<command>],
                              [Specify command to launch MPI parallel tests.])],
            [MPIEXEC=$with_mpiexec], [MPIEXEC=mpiexec])
AC_MSG_RESULT([$MPIEXEC])
AC_SUBST([MPIEXEC], [$MPIEXEC])

# We need the math library
AC_CHECK_LIB([m], [floor], [],
[AC_MSG_ERROR([Can't find or link to the math library.])])

# Check for netCDF C library.
AC_SEARCH_LIBS([nc_create], [netcdf], [],
                            [AC_MSG_ERROR([Can't find or link to the netCDF C library, set CPPFLAGS/LDFLAGS.])])
AC_CHECK_HEADERS([netcdf.h netcdf_meta.h])

# Do we have netCDF-4? If not, we're done.
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include "netcdf_meta.h"],
[[#if !NC_HAS_NC4
# error
#endif]
])], [have_netcdf4=yes], [have_netcdf4=no])
AC_MSG_CHECKING([whether netCDF provides netCDF/HDF5])
AC_MSG_RESULT([${have_netcdf4}])
if test "x$have_netcdf4" = xno; then
   AC_MSG_ERROR([NetCDF-4 is required])
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
AC_SUBST(HAS_NETCDF_PAR,[$have_netcdf_par])
AM_CONDITIONAL(HAVE_NETCDF_PAR, [test "x$have_netcdf_par" = xyes])

# If user asked for parallel tests, and parallel is not available, error.
if test "x$enable_parallel_tests" = xyes -a "x$have_netcdf_par" != xyes; then
   AC_MSG_ERROR([parallel tests requested, but netcdf-c does not provide parallel I/O.])
fi

# Do we have szip?
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([#include "netcdf_meta.h"],
[[#if !NC_HAS_SZIP_WRITE
# error
#endif]
])], [have_szip_write=yes], [have_szip_write=no])
AC_MSG_CHECKING([whether netCDF provides szip write capability])
AC_MSG_RESULT([${have_szip_write}])

# Do we have parallel filter support?
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
AC_SUBST(HAS_PAR_FILTERS,[$have_par_filters])

# Does netcdf-c support multiple filters (version 4.8.0 or later).
AC_MSG_CHECKING([if netCDF handles multiple filters])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [[
#include <netcdf_meta.h>
#if !defined(NC_HAS_MULTIFILTERS) || NC_HAS_MUITIFILTERS == 0
      choke me
#endif]])], [have_multifilters=yes], [have_multifilters=no])
if test "x$have_multifilters" = xyes; then
   AC_DEFINE([HAVE_MULTIFILTERS], [1], [if true, netCDF-C supports multiple filters])
fi
AM_CONDITIONAL(HAVE_MULTIFILTERS, [test "x$have_multifilters" = xyes])
AC_SUBST(HAS_MULTIFILTERS,[$have_multifilters])
AC_MSG_RESULT([$have_multifilters])

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

# Configure the test running scripts.
AC_CONFIG_FILES([test/run_tests.sh], [chmod ugo+x test/run_tests.sh])
AC_CONFIG_FILES([test/run_par_tests.sh], [chmod ugo+x test/run_par_tests.sh])
AC_CONFIG_FILES([ftest/run_tests.sh], [chmod ugo+x ftest/run_tests.sh])
AC_CONFIG_FILES([test_h5/run_tests.sh], [chmod ugo+x test_h5/run_tests.sh])

# Build the HDF5 plugins.
AC_CONFIG_SUBDIRS([hdf5_plugins])

# Generate the config.h file.
AC_CONFIG_HEADERS([config.h])

# Populate ccr-config
# Flags for nc-config script; by design $prefix, $includir, $libdir,
# etc.  are left as shell variables in the script so as to facilitate
# relocation
if test "x$with_netcdf_c_lib" = x ; then
   NC_LIBS="-lnetcdf"
else
   NC_LIBS="$with_netcdf_c_lib"
fi
if test "x$enable_shared" != xyes; then
   NC_LIBS="$LDFLAGS $NC_LIBS $LIBS"
fi
CCR_LIBS="-lccr"
CCR_FLIBS="-lccrf"

AC_SUBST(NC_LIBS,[$NC_LIBS])
AC_SUBST(CCR_LIBS,[$CCR_LIBS])
AC_SUBST(CCR_FLIBS,[$CCR_FLIBS])
AC_SUBST(BUILD_FORTRAN,[$enable_fortran])
AC_CONFIG_FILES([ccr-config], [chmod 755 ccr-config])

# Create output variables from various shell variables, for use in
# generating libccr.settings.
AC_SUBST([enable_shared])
AC_SUBST([enable_static])
AC_SUBST([CFLAGS])
AC_SUBST([CPPFLAGS])
AC_SUBST([LDFLAGS])
AC_SUBST([HDF5_PLUGIN_PATH])

# Args:
# 1. ccr_meta.h variable
# 2. conditional variable that is yes or no.
# 3. default condition
#
# example: AX_SET_META([CCR_HAS_NC2],[$ccr_build_v2],[]) # Because it checks for no.
#          AX_SET_META([CCR_HAS_HDF4],[$enable_hdf4],[yes])
AC_DEFUN([AX_SET_META],[
  if [ test "x$2" = x$3 ]; then
     AC_SUBST([$1]) $1=1
  else
     AC_SUBST([$1]) $1=0
  fi
])

# Settings for libccr.settings and ccr_meta.h.
AC_SUBST(HAS_LZ4,[$enable_lz4])
AX_SET_META([CCR_HAS_LZ4],[$enable_lz4],[yes])
AC_SUBST(HAS_ZSTD,[$enable_zstd])
AX_SET_META([CCR_HAS_ZSTD],[$enable_zstd],[yes])
AC_SUBST(HAS_BITGROOM,[$enable_bitgroom])
AX_SET_META([CCR_HAS_BITGROOM],[$enable_bitgroom],[yes])
AC_SUBST(HAS_BZIP2,[$enable_bzip2])
AX_SET_META([CCR_HAS_BZIP2],[$enable_bzip2],[yes])

AX_SET_META([CCR_HAS_NETCDF_PAR],[$have_netcdf_par],[yes])
AX_SET_META([CCR_HAS_PAR_FILTERS],[$have_par_filters],[yes])
AX_SET_META([CCR_HAS_MULTIFILTERS],[$have_multifilters],[yes])

# THis is the text summary and header file of how CCR was built.
AC_CONFIG_FILES([libccr.settings
                 include/ccr_meta.h
		 ])
		 
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

# Print build summary.
cat libccr.settings
