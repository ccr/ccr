# This is the Makefile.am for the HDF5 BitRound filter library
# This allows the use of BitRound quantization on HDF5 datasets
#
# Charlie Zender 9/12/20, 1/26/22

# No extra paths necessary since BitRound filter source is in plugin code
# AM_CPPFLAGS = -I$(HDF5_ROOT)/include -I$(BITROUND_ROOT)/include

# This is where HDF5 wants us to install plugins
plugindir = @HDF5_PLUGIN_PATH@

# This linker flag specifies libtool version info.
# See http://www.gnu.org/software/libtool/manual/libtool.html#Libtool-versioning
# for information regarding incrementing `-version-info`.
libh5btr_la_LDFLAGS = -version-info 0:0:0

# The libh5btr library for plugin module
# Build it as shared library
plugin_LTLIBRARIES = libh5btr.la
libh5btr_la_SOURCES = H5Zbitround.c
