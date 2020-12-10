# This is an automake file for the netCDF Community Codec Repository
# (CCR).

# Ed Hartnett 12/24/2019, Charlie Zender 9/20/20

# This directory stores libtool macros, put there by aclocal.
ACLOCAL_AMFLAGS = -I m4

# Does the user want to build documentation?
if BUILD_DOCS
DOCS = docs
endif

# Does the user want the Fortran API?
if BUILD_FORTRAN
FSRC = fsrc
FTEST = ftest
endif

# Run make in these subdirectories.
SUBDIRS = hdf5_plugins include src $(FSRC) $(FTEST) test_h5 test	\
$(DOCS)

# install libccr.settings in lib directory.
settingsdir = $(libdir)
settings_DATA = libccr.settings

DISTCLEANFILES = libccr.settings

EXTRA_DIST = LICENSE README.md libccr.settings.in
