# This is part of ncglm. It creates the doc Makefile.

# Ed Hartnett 11/11/19

# Run doxygen, then confirm warning log file is empty.
all:
	doxygen Doxyfile
	cat doxywarn.log
	[ ! -s doxywarn.log ]

check: all

CLEANFILES = *.log

EXTRA_DIST = Doxyfile.in
