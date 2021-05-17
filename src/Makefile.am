# This file builds the src directory for the ccr library.

# Ed Hartnett 12/24/2019

# Find the header files.
AM_CPPFLAGS = -I$(top_srcdir)/include

# This is a libtool library.
lib_LTLIBRARIES = libccr.la
libccr_la_LDFLAGS = -version-info 1:1:0
libccr_la_SOURCES = ccr.c
