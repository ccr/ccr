# This is the main configure file for the netCDF Community Codec
# Repository (ccr).

# Ed Hartnett 12/24/2019

AC_PREREQ([2.59])

# Initialize with name, version, and support email address.
AC_INIT([ccr], [1.0-development], [])

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

# Does the user want to enable Fortran library?
AC_MSG_CHECKING([whether Fortran library should be built])
AC_ARG_ENABLE([fortran],
              [AS_HELP_STRING([--enable-fortran],
                              [build the ncglm Fortran library.])])
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

# We need the math library
AC_CHECK_LIB([m], [floor], [],
[AC_MSG_ERROR([Can't find or link to the math library.])])

# Check for netCDF C library.
AC_SEARCH_LIBS([nc_create], [netcdf], [],
                            [AC_MSG_ERROR([Can't find or link to the netcdf C library, set CPPFLAGS/LDFLAGS.])])

# Check for netCDF Fortran library.
if test "x$enable_fortran" = xyes; then
   AC_LANG_PUSH(Fortran)
   AC_SEARCH_LIBS([nf_create], [netcdff], [], [AC_MSG_ERROR([Can't find or link to the netcdf Fortran library, set CPPFLAGS/LDFLAGS.])])
   AC_LANG_POP(Fortran)
fi

# Build the HDF5 plugins.
AC_CONFIG_SUBDIRS([hdf5_plugins])

# These files will be created when the configure script is run.
AC_CONFIG_FILES([Makefile
	src/Makefile
	include/Makefile
	test_h5/Makefile
	docs/Makefile
        ])
AC_OUTPUT()
