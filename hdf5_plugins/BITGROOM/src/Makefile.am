

AM_CPPFLAGS = -I$(HDF5_ROOT)/include -I$(BITGROOM_ROOT)/include
plugindir = /usr/local/hdf5/lib/plugin

# The libh5bitgroom library for plugin module.
# Build it as shared library.
lib_LTLIBRARIES = libh5bitgroom.la
libh5bitgroom_la_SOURCES = H5Zbitgroom.c

LDFLAGS = -L$(BITGROOM_ROOT)/lib
LIBS = -lbitgroom

