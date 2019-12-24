##                                               -*- Autoconf -*-
## Process this file with autoconf to produce a configure script.
##
## Copyright by The HDF Group.
## Copyright by the Board of Trustees of the University of Illinois.
## All rights reserved.
##
## This file is part of HDF5.  The full HDF5 copyright notice, including
## terms governing use, modification, and redistribution, is contained in
## the files COPYING and Copyright.html.  COPYING can be found at the root
## of the source code distribution tree; Copyright.html can be found at the
## root level of an installed copy of the electronic HDF5 document set and
## is linked from the top-level documents page.  It can also be found at
## http://hdfgroup.org/HDF5/doc/Copyright.html.  If you do not have
## access to either file, you may request a copy from help@hdfgroup.org.

AC_PREREQ(2.59)
AC_INIT(H5BZ2, 0.1, help@hdfgroup.org)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([bin])
## AM_INIT_AUTOMAKE takes a list of options that should be applied to
## every Makefile.am when automake is run.
AM_INIT_AUTOMAKE([foreign serial-tests])
AM_CONFIG_HEADER(config.h)

## Turn off automake rebuild rules so make doesn't try to run automake
## (which probably won't work).
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_INSTALL

AC_CANONICAL_HOST
AC_PREFIX_DEFAULT([`pwd`/plugins])

LT_INIT

## Source any special files that we need. These files normally aren't
## present but can be used by the maintainers to fine tune things like
## turning on debug or profiling flags for the compiler. The search order
## is:
##
##         CPU-VENDOR-OS
##         VENDOR-OS
##         CPU-OS
##         CPU-VENDOR
##         OS
##         VENDOR
##         CPU
##
## If the `OS' ends with a version number then remove it. For instance,
## `freebsd3.1' would become `freebsd'
##
case "$host_os" in
  aix4.*)       host_os_novers="aix4.x"     ;;
  aix5.*)       host_os_novers="aix5.x"     ;;
  freebsd*)     host_os_novers="freebsd"    ;;
  irix5.*)      host_os_novers="irix5.x"    ;;
  irix6.*)      host_os_novers="irix6.x"    ;;
  osf4.*)       host_os_novers="osf4.x"     ;;
  osf5.*)       host_os_novers="osf5.x"     ;;
  solaris2.*)   host_os_novers="solaris2.x" ;;
  *)            host_os_novers="$host_os"   ;;
esac

host_config="none"
for f in $host_cpu-$host_vendor-$host_os        \
         $host_cpu-$host_vendor-$host_os_novers \
         $host_vendor-$host_os                  \
         $host_vendor-$host_os_novers           \
         $host_cpu-$host_os                     \
         $host_cpu-$host_os_novers              \
         $host_cpu-$host_vendor                 \
         $host_os                               \
         $host_os_novers                        \
         $host_vendor                           \
         $host_cpu ; do
  AC_MSG_CHECKING([for config $f])
  if test -f "$srcdir/config/$f"; then
    host_config=$srcdir/config/$f
    AC_MSG_RESULT([found])
    break
  fi
  AC_MSG_RESULT([no])
done
if test "X$host_config" != "Xnone"; then
  CC_BASENAME="`echo $CC | cut -f1 -d' ' | xargs basename 2>/dev/null`"
  . $host_config
fi

## Fix up the INSTALL macro if its a relative path. We want the
## full-path to the binary instead.
case "$INSTALL" in
  *install-sh*)
    INSTALL='\${top_srcdir}/bin/install-sh -c'
    ;;
esac

## ----------------------------------------------------------------------
## Check which archiving tool to use. This needs to be done before
## the AC_PROG_LIBTOOL macro.
##
if test -z "$AR"; then
  AC_CHECK_PROGS([AR], [ar xar], [:], [$PATH])
fi

AC_SUBST([AR])

## Export the AR macro so that it will be placed in the libtool file
## correctly.
export AR

AC_PROG_MAKE_SET
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

## Post processing to patch up some deficiencies in libtool
case $host_os in
  linux*)
    # If gcc is not used, need to set $wl to use "-Wl,"
    if $CC -v 2>&1 | grep '^gcc' > /dev/null ; then
      : using gcc
    else
      echo 'fixing $wl in' $ofile
ed - $ofile <<EOF 2> /dev/null
/^wl=""/s//wl="-Wl,"/
w
q
EOF

    fi
    ;;
esac

## HDF5 compile commands.
AC_SUBST([H5CC])
AC_SUBST([H5CC_PP])
AC_SUBST([H5FC])
AC_SUBST([H5FC_PP])
AC_SUBST([H5CPP])

## Make sure they are set to something.
H5CC="${H5CC:-h5cc}"
H5CC_PP="${H5CC_PP:-h5pcc}"
H5FC="${H5FC:-h5fc}"
H5FC_PP="${H5FC_PP:-h5pfc}"
H5CPP="${H5CPP:-h5c++}"

## ----------------------------------------------------------------------
## Fake --with-xxx option to allow us to create a help message for the
## following --with-xxx options which can take either a =DIR or =INC,LIB
## specifier.
##
AC_ARG_WITH([fnord],
  [
 For the following --with-xxx options, you can specify where the header
 files and libraries are in two different ways:

    --with-xxx=INC,LIB - Specify individually the include directory and
                         library directory separated by a comma
    --with-xxx=DIR     - Specify only the directory which contains the
                         include/ and lib/ subdirectories
  ],,)


## ----------------------------------------------------------------------
## Is the bzip2 library present? It has a header file `bzlib.h' and a library
## `-lbz2' and their locations might be specified with the `--with-bz2lib'
## command-line switch. The value is an include path and/or a library path.
## If the library path is specified then it must be preceded by a comma.
##
AC_ARG_WITH([bz2lib],
            [AS_HELP_STRING([--with-bz2lib=DIR],
                            [Use bz2lib library for compressed I/O
                             filter [default=yes]])],,)

case $withval in
  yes)
    HAVE_BZ2LIB="yes"
    AC_CHECK_HEADER([bzlib.h], [HAVE_BZ2LIB_H="yes"], [unset HAVE_BZ2LIB])
    if test "x$HAVE_BZ2LIB" = "xyes" -a "x$HAVE_BZ2LIB_H" = "xyes"; then
      AC_CHECK_LIB([bz2], [BZ2_bzCompress],, [unset HAVE_BZ2LIB])
    fi
    if test -z "$HAVE_BZ2LIB"; then
      AC_MSG_ERROR([couldn't find bz2lib library])
    else
      AC_CHECK_FUNC([BZ2_bzCompress], [HAVE_BZ2_COMPRESS="yes"])
    fi
    ;;
  no)
    HAVE_BZ2LIB="no"
    AC_MSG_ERROR([bz2 library needed for compilation.])
    ;;
  *)
    HAVE_BZ2LIB="yes"
    case "$withval" in
      *,*)
        bz2lib_inc="`echo $withval | cut -f1 -d,`"
        bz2lib_lib="`echo $withval | cut -f2 -d, -s`"
        ;;
      *)
        if test -n "$withval"; then
          bz2lib_inc="$withval/include"
          bz2lib_lib="$withval/lib"
        fi
        ;;
    esac

    ## Trying to include -I/usr/include and -L/usr/lib is redundant and
    ## can mess some compilers up.
    if test "X$bz2lib_inc" = "X/usr/include"; then
      bz2lib_inc=""
    fi
    if test "X$bz2lib_lib" = "X/usr/lib"; then
      bz2lib_lib=""
    fi

    saved_CPPFLAGS="$CPPFLAGS"
    saved_AM_CPPFLAGS="$AM_CPPFLAGS"
    saved_LDFLAGS="$LDFLAGS"
    saved_AM_LDFLAGS="$AM_LDFLAGS"

    if test -n "$bz2lib_inc"; then
      CPPFLAGS="$CPPFLAGS -I$bz2lib_inc"
      AM_CPPFLAGS="$AM_CPPFLAGS -I$bz2lib_inc"
    fi

    AC_CHECK_HEADER([bzlib.h],
                     [HAVE_BZ2LIB_H="yes"],
                     [CPPFLAGS="$saved_CPPFLAGS"; AM_CPPFLAGS="$saved_AM_CPPFLAGS"] [unset HAVE_BZ2LIB])

    if test -n "$bz2lib_lib"; then
      LDFLAGS="$LDFLAGS -L$bz2lib_lib"
      AM_LDFLAGS="$AM_LDFLAGS -L$bz2lib_lib"
    fi

    if test "x$HAVE_BZ2LIB" = "xyes" -a "x$HAVE_BZ2LIB_H" = "xyes"; then
      AC_CHECK_LIB([bz2], [BZ2_bzCompress],,
                   [LDFLAGS="$saved_LDFLAGS"; AM_LDFLAGS="$saved_AM_LDFLAGS"; unset HAVE_BZ2LIB])
    fi

    if test -z "$HAVE_BZ2LIB"; then
      AC_MSG_ERROR([couldn't find bz2lib library])
    else
      AC_CHECK_FUNC([BZ2_bzCompress], [HAVE_BZ2_COMPRESS="yes"])
    fi
    ;;
esac

## ----------------------------------------------------------------------
## Where's HDF5? We need to find out where the HDF5 library
## and header files are if they aren't in system library/include places.
##
AC_SUBST(LIBHDF5) LIBHDF5=""
AC_SUBST(HDF5PATH) HDF5PATH=""
AC_ARG_WITH([hdf5],
            [AC_HELP_STRING([--with-hdf5=DIR],
                            [Where to find the HDF5 library])],,)

case "$withval" in
  yes)
    AC_CHECK_HEADER([hdf5.h],,
                     [AC_MSG_ERROR([cannot find HDF5 header files.])])
    AC_CHECK_LIB([hdf5], [H5open],,
                 [AC_MSG_ERROR([cannot find the HDF5 library.])])
    ;;
  no)
    AC_MSG_ERROR([HDF5 library needed for compilation.])
    ;;
  *)
    case "$withval" in
      *,*)
        hdf5_inc="`echo $withval | cut -f1 -d,`"
        hdf5_lib="`echo $withval | cut -f2 -d, -s`"
        ;;
      *)
        if test -n "$withval"; then
          hdf5_inc="$withval/include"
          hdf5_lib="$withval/lib"
        fi
        ;;
    esac

    if test -n "$hdf5_inc"; then
      CPPFLAGS="$CPPFLAGS -I$hdf5_inc"
    fi

    AC_CHECK_HEADER([hdf5.h],,
                     [AC_MSG_ERROR([cannot find HDF5 header files!])])

    HDF5PATH="$hdf5_lib,"
    HDF5PATH="`echo $HDF5PATH | sed -e "s/lib,/bin/"`"

    if test -n "$hdf5_lib"; then
      LDFLAGS="$LDFLAGS -L$hdf5_lib"

      ## We want to use the ``.la'' version of the HDF5 library which
      ## is the thing libtool creates and has in it all of the neat
      ## stuff like where to find the libraries libhdf5 depends upon.
      if test -f "$hdf5_lib/libhdf5.la"; then
        libdir_saved=$libdir
        . $hdf5_lib/libhdf5.la
        libdir=$libdir_saved
        for lib in X $dependency_libs; do
          case "$lib" in
            *libhdf5.la*)
              ;;
            *)
              if test "$lib" != "X"; then
                LIBHDF5="$LIBHDF5 $lib"
              fi
              ;;
          esac
        done
      fi
    fi

    AC_CHECK_LIB([hdf5], [H5open],,
                 [AC_MSG_ERROR([cannot find HDF5 library.])])
    ;;
esac

H5CC=${HDF5PATH}/h5cc
H5FC=${HDF5PATH}/h5fc

## Add the suffix to CC for shared linking.  Cannot just set as an option
## because it must be first.
if test "$shared_suffix" && test ! `echo $CC | grep "$shared_suffix"`; then
    CC=${CC}${shared_suffix}
fi

## ======================================================================
## Checks for library functions
## ======================================================================

AC_MSG_CHECKING([for math library support])
AC_TRY_LINK([#include <math.h>], [volatile x = 37.927; k=1.2; sinh(x*k/cosh(x/k)],
            [AC_MSG_RESULT([yes])],
            [AC_MSG_RESULT([no]); LIBS="$LIBS -lm"])


## Checks for libraries.

## Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h])

## Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

## Checks for library functions.
AC_FUNC_ERROR_AT_LINE
##AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memset])

## Check which plugins to build, if no environmental variables are set, build
## all.
if test ! "$PLUGIN_H5BZ2"
then
  PLUGIN_H5BZ2=1
fi
AM_CONDITIONAL(H5BZ2, test "$PLUGIN_H5BZ2")

## Check if a Fortran 2003 compiler was used for h5fc
HAVE_FORTRAN_2003="no"

AC_MSG_CHECKING([if h5fc was compiled with Fortran 2003 enabled])
if (${FC} -showconfig 2>&1 | grep 'Fortran 2003 Compiler: yes') > /dev/null; then
  HAVE_FORTRAN_2003="yes"
fi
echo $HAVE_FORTRAN_2003

## Check if we have Fortran 2003 and Intel compiler; Intel does not work with RECURSIVE used by h5ex_g_traverse.f90
## The example will not be built when Intel compiler is used (EIP 2011/10/14)

if test "X$HAVE_FORTRAN_2003" = "Xyes"; then
  HAVE_FORTRAN_2003_NOTINTEL="yes"
  AC_MSG_CHECKING([if h5fc is an Intel Fortran compiler])
  if (${H5FC} -showconfig 2>&1 | grep 'Intel(R) Fortran ') > /dev/null; then
    HAVE_FORTRAN_2003_NOTINTEL="no"
  fi
  if test "X$HAVE_FORTRAN_2003_NOTINTEL" = "Xyes"; then
    echo "no"
  else
    echo "yes"
  fi
fi
## End check if we have Fortran 2003 and Intel compiler.

AM_CONDITIONAL([FORTRAN_2003_CONDITIONAL_F], [test "X$HAVE_FORTRAN_2003" = "Xyes"])
AM_CONDITIONAL([FORTRAN_2003_NOTINTEL_CONDITIONAL_F], [test "X$HAVE_FORTRAN_2003_NOTINTEL" = "Xyes"])

LT_OUTPUT

## Set subdirectories
AC_CONFIG_FILES([Makefile
        src/Makefile])

## Configure
AC_OUTPUT
