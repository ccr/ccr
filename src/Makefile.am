# This file builds the src directory for the ccr library.

# Ed Hartnett 12/24/2019

# Find the header files.
AM_CPPFLAGS = -I$(top_srcdir)/include

AM_LDFLAGS = ${top_builddir}/hdf5_plugins/BZIP2/src/libh5bz2.la ${top_builddir}/hdf5_plugins/LZ4/src/libh5lz4.la
LDADD = ${top_builddir}/hdf5_plugins/BZIP2/src/libh5bz2.la ${top_builddir}/hdf5_plugins/LZ4/src/libh5lz4.la

# This is a libtool library.
lib_LTLIBRARIES = libccr.la
libccr_la_LDFLAGS = -version-info 0:0:0
libccr_la_SOURCES = ccr.c
