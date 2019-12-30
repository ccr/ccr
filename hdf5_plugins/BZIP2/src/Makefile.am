

AM_CPPFLAGS = -I$(HDF5_ROOT)/include -I$(BZ2_ROOT)/include
plugindir = /usr/local/hdf5/lib/plugin

# The libh5bz2 library for plugin module.
# Build it as shared library.
lib_LTLIBRARIES = libh5bz2.la
libh5bz2_la_SOURCES = H5Zbzip2.c

LDFLAGS = -L$(BZ2_ROOT)/lib
LIBS = -lbz2

