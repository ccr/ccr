#
# Copyright by The HDF Group.
# All rights reserved.
#
# HDF5 BZIP2 Library Plugin makefile(.in)
#
# Allen Byrne, Ed Hartnett, 1/12/19

AM_CPPFLAGS = -I$(HDF5_ROOT)/include -I$(BZ2_ROOT)/include

# This is where HDF5 wants us to install plugins.
plugindir = @HDF5_PLUGIN_PATH@

# This linker flag specifies libtool version info.
# See http://www.gnu.org/software/libtool/manual/libtool.html#Libtool-versioning
# for information regarding incrementing `-version-info`.
libh5bz2_la_LDFLAGS = -version-info 0:0:0

# The libh5bz2 library for plugin module.
# Build it as shared library.
plugin_LTLIBRARIES = libh5bz2.la
libh5bz2_la_SOURCES = H5Zbzip2.c
