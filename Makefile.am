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

# The ccr-config script helps the user build programs with netCDF.
bin_SCRIPTS = ccr-config

DISTCLEANFILES = libccr.settings

EXTRA_DIST = LICENSE README.md libccr.settings.in ccr-config.in

install-data-hook:

	@echo ''
	@echo '+-------------------------------------------------------------+'
	@echo '| Congratulations! You have successfully installed CCR!       |'
	@echo '|                                                             |'
	@echo '| You can use script "ccr-config" to find out the relevant    |'
	@echo '| compiler options to build your application. Enter           |'
	@echo '|                                                             |'
	@echo '|     ccr-config --help                                       |'
	@echo '|                                                             |'
	@echo '| for additional information.                                 |'
	@echo '|                                                             |'
	@echo '| CAUTION:                                                    |'
	@echo '|                                                             |'
	@echo '| If you have not already run "make check", then we strongly  |'
	@echo '| recommend you do so. It does not take very long.            |'
	@echo '|                                                             |'
	@echo '| Before using CCR to store important data, test your         |'
	@echo '| build with "make check".                                    |'
	@echo '|                                                             |'
	@echo '| If any tests fail, please see the CCR web site:             |'
	@echo '| https://github.com/ccr/ccr                                  |'
	@echo '+-------------------------------------------------------------+'
	@echo ''

