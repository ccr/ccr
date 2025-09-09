# Copyright by The HDF Group. All rights reserved.

# This file builds main directory for the hdf5_plugins project.

# Ed Hartnett 12/24/2019

# This directory stores libtool macros, put there by aclocal.
ACLOCAL_AMFLAGS = -I m4

# Does the user want to build lz4?
if BUILD_LZ4
LZ4 = LZ4
endif

# Build the desired subdirectories.
SUBDIRS = $(LZ4)
