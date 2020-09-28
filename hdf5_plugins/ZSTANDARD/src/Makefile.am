# This is the Makefile.am for the HDF5 Zstandard filter library
# This allows the use of Zstandard compression on HDF5 datasets
#
# Charlie Zender 9/15/20

# Add any paths necessary to find HDF5 and Zstandard library headers
# AM_CPPFLAGS = -I$(HDF5_ROOT)/include -I$(ZSTD_ROOT)/include

# This is where HDF5 wants us to install plugins
plugindir = @HDF5_PLUGIN_PATH@

# This linker flag specifies libtool version info.
# See http://www.gnu.org/software/libtool/manual/libtool.html#Libtool-versioning
# for information regarding incrementing `-version-info`.
libh5zstd_la_LDFLAGS = -version-info 0:0:0

# The libh5zstd library for plugin module
# Build it as shared library
plugin_LTLIBRARIES = libh5zstd.la
libh5zstd_la_SOURCES = H5Zzstandard.c
