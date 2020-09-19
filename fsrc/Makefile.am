j# This file builds the fsrc directory for the CCR library.

# Ed Hartnett 1/23/2020

# Find the header files.
AM_CPPFLAGS = -I$(top_srcdir)/include

#AM_LDFLAGS = ${top_builddir}/hdf5_plugins/BZIP2/src/libh5bz2.la ${top_builddir}/hdf5_plugins/LZ4/src/libh5lz4.la${top_builddir}/hdf5_plugins/LZ4/src/libh5lz4.la ${top_builddir}/hdf5_plugins/LZ4/src/libh5bgr.la ${top_builddir}/hdf5_plugins/LZ4/src/libh5zstd.la
LIBADD = ${top_builddir}/src/libccr.la

# This is a libtool library.
lib_LTLIBRARIES = libccrf.la
libccrf_la_LDFLAGS = -version-info 0:0:0

# Each convenience library depends on its source.
libccrf_la_SOURCES = ccr.F90

# Each mod file depends on the .o file.
ccr.mod: ccr.$(OBJEXT)

# Mod files are built and then installed as headers.
MODFILES = ccr.mod
include_HEADERS = $(MODFILES)
BUILT_SOURCES = $(MODFILES)

CLEANFILES = *.mod
