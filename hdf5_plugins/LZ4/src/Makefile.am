#
# Copyright by The HDF Group.
# All rights reserved.
#
##
## Makefile.am
## Run automake to generate a Makefile.in from this file.
##
#
# HDF5 LZ4 Library Plugin makefile(.in)
#

# Shell commands used in Makefiles
RM=rm -f
CP=cp

MAJOR = 0
MINOR = 1
VERSION = $(MAJOR).$(MINOR)

AM_CPPFLAGS=-I$(HDF5_ROOT)/include -I$(LZ4_ROOT)/include

# The libh5lz4 library for plugin module.
# Build it as shared library.
lib_LTLIBRARIES=libh5lz4.la
libh5lz4_la_SOURCES=H5Zlz4.c

LDFLAGS=-L$(LZ4_ROOT)/lib
LIBS=-llz4

CHECK_CLEANFILES=*.o *.so* 
DISTCLEANFILES=*.o *.so* 
