# This is an automake file for the NetCDF Community Codec Repository
# (crr).

# Ed Hartnett 12/24/2019

# This directory stores libtool macros, put there by aclocal.
ACLOCAL_AMFLAGS = -I m4

# Does the user want to build documentation?
if BUILD_DOCS
DOCS = docs
endif

# Does the user want the Fortran API?
# if BUILD_FORTRAN
# FSRC = fsrc
# FTEST = ftest
# endif

SUBDIRS = include src test_h5 ${DOCS}

EXTRA_DIST = test-driver-verbose LICENSE README.md
