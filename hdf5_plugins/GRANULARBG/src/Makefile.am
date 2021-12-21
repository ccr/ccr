# This is the Makefile.am for the HDF5 Granular BitGroom filter library
# This allows the use of Granular BitGroom quantization on HDF5 datasets
#
# Charlie Zender 10/17/20

# No extra paths necessary since Granular BitGroom filter source is in plugin code
# AM_CPPFLAGS = -I$(HDF5_ROOT)/include -I$(BITGROOM_ROOT)/include

# This is where HDF5 wants us to install plugins
plugindir = @HDF5_PLUGIN_PATH@

# This linker flag specifies libtool version info.
# See http://www.gnu.org/software/libtool/manual/libtool.html#Libtool-versioning
# for information regarding incrementing `-version-info`.
libh5gbg_la_LDFLAGS = -version-info 0:0:0

# The libh5gbg library for plugin module
# Build it as shared library
plugin_LTLIBRARIES = libh5gbg.la
libh5gbg_la_SOURCES = H5Zgranularbg.c
