#
# Copyright by The HDF Group.
# All rights reserved.
#
##
## Makefile.am
## Run automake to generate a Makefile.in from this file.
##
#
# HDF5 BZIP2 Library Plugin makefile(.in)
#

# Shell commands used in Makefiles
# RM=rm -f
# CP=cp

# MAJOR = 0
# MINOR = 1
# VERSION = $(MAJOR).$(MINOR)

AM_CPPFLAGS = -I$(HDF5_ROOT)/include -I$(BZ2_ROOT)/include

# The libh5bz2 library for plugin module.
# Build it as shared library.
lib_LTLIBRARIES = libh5bz2.la
libh5bz2_la_SOURCES = H5Zbzip2.c

LDFLAGS = -L$(BZ2_ROOT)/lib
LIBS = -lbz2

