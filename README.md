# Community Codec Repository

This project supports compression (and other) filters for netCDF/HDF5
files which are not natively supported by the netCDF C library.

The netCDF C library supports zlib and (optionally) szlib as
compression filters. The C library also supports the shuffle and
checksum filters natively, and supports other filters via the recently
added nc_def_var_filter() method (in version 4.7.3 and later).

Additional filters are available, and support additional compression
methods.

Version 1.0 of the CCR supports:
* BZIP2 compression
* LZ4 compression


