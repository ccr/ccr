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

For full documentation see https://ccr.github.io/ccr/.

# Building CCR

## Dependencies

CCR depends on netcdf-c, hdf5, and (optionally) third-party
compression libraries. These libraries must be installed on the target
system before CCR is built.

Library   | Source                                    | Notes
--------- |-------                                    | -----
netcdf-c  | https://github.com/Unidata/netcdf-c       | required
HDF5      | https://www.hdfgroup.org/downloads/hdf5   | required
bzip2     | https://www.sourceware.org/bzip2/         | optional
LZ4       | https://github.com/lz4/lz4                | optional
Zstandard | https://facebook.github.io/zstd/          | optional 

### Obtain Optional External Libraries as Pre-built Packages

Codec     |  Environment | Install Command
--------- |------------- | ---------------
LZ4       |  CentOS      | sudo yum install lz4-devel
LZ4       |  Conda       | conda install lz4
LZ4       |  Debian      | sudo aptitude install liblz4-dev
LZ4       |  Fedora      | sudo dnf install lz4-devel
LZ4       |  MacPorts    | sudo port install lz4
Zstandard |  CentOS      | sudo yum install libzstd-devel
Zstandard |  Conda       | conda install zstd
Zstandard |  Debian      | sudo aptitude install libzstd1-dev
Zstandard |  Fedora      | sudo dnf install libzstd-devel
Zstandard |  MacPorts    | sudo port install zstd

## Autotools Build

To build, first run the configure script. The configure script will
try to locate all the necessary dependencies. Use the CPPFLAGS and
LDFLAGS environment variables to specify the locations of include and
library files for the third-party libraries.

Example:
<pre>
export CFLAGS='-g -Wall'
export CPPFLAGS='-I/usr/local/hdf5-1.10.6_mpich/include -I/usr/local/netcdf-c-4.7.4_hdf5-1.10.6_szip_mpich/include'
export LDFLAGS='-L/usr/local/hdf5-1.10.6_mpich/lib -L/usr/local/netcdf-c-4.7.4_hdf5-1.10.6_szip_mpich/lib'
./configure  --disable-lz4 --disable-zstd 
</pre>

Build the CCR code with make, and run tests with make check.

# REFERENCES

Delaunay, X., A. Courtois, and F. Gouillon (2019), Evaluation of
lossless and lossy algorithms for the compression of scientific
datasets in netCDF-4 or HDF5 files, Geosci. Model Dev., 12(9),
4099-4113, doi:10.5194/gmd-2018-250, retrieved on Sep 21, 2020 from
https://www.researchgate.net/publication/335987647_Evaluation_of_lossless_and_lossy_algorithms_for_the_compression_of_scientific_datasets_in_netCDF-4_or_HDF5_files.

Hartnett, E. (2011), netCDF-4/HDF5 File Format,
https://earthdata.nasa.gov/files/ESDS-RFC-022v1.pdf

Zender, C. S. (2016), Bit Grooming: Statistically accurate
precision-preserving quantization with compression, evaluated in the
netCDF Operators (NCO, v4.4.8+), Geosci. Model Dev., 9, 3199-3211,
doi:10.5194/gmd-9-3199-2016 Retrieved on Sep 21, 2020 from
https://www.researchgate.net/publication/301575383_Bit_Grooming_Statistically_accurate_precision-preserving_quantization_with_compression_evaluated_in_the_netCDF_Operators_NCO_v448.
