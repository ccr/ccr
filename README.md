# Community Codec Repository

This project supports compression (and other) filters for netCDF/HDF5
files which are not natively supported by the netCDF C library.

The [netCDF C library](https://github.com/Unidata/netcdf-c) supports
zlib and (optionally) szlib as compression filters. The C library also
supports the shuffle and checksum filters natively, and supports other
filters via the recently added nc_def_var_filter() method (in version
4.7.3 and later).

Additional filters are available, and support additional compression
methods.

Version 1.3.0 of the CCR supports:
* BZIP2 compression
* Zstandard compression
* BitGroom pre-compression
* Granular BitRound pre-compression

For full documentation see https://ccr.github.io/ccr/.

# Authors

Charlie Zender, Edward Hartnett

Filter | Author
-------|-------
Bzip2  | Francesc Alted, Carabos Coop. V., HDF Team
Zstandard | Yann Collet
BitGroom | Charlie Zender
Granular BitRound | Charlie Zender
BitRound | Charlie Zender

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
Zstandard | https://facebook.github.io/zstd/          | optional 

### Obtain Optional External Libraries as Pre-built Packages

Codec     |  Environment | Install Command
--------- |------------- | ---------------
Zstandard |  CentOS      | sudo yum install libzstd-devel
Zstandard |  Conda       | conda install zstd
Zstandard |  Debian      | sudo aptitude install libzstd1-dev
Zstandard |  Fedora      | sudo dnf install libzstd-devel
Zstandard |  MacPorts    | sudo port install zstd

## Autotools Build

Download the CCR release and unpack it. Run autoreconf, configure, and make.

Example:
<pre>
# Create configure script
autoreconf -i
# Set your environment as necessary and (re-)configure as necessary:
export CFLAGS='-g -Wall'
export CPPFLAGS='-I/usr/local/hdf5-1.10.6_mpich/include -I/usr/local/netcdf-c-4.7.4_hdf5-1.10.6_szip_mpich/include'
export LDFLAGS='-L/usr/local/hdf5-1.10.6_mpich/lib -L/usr/local/netcdf-c-4.7.4_hdf5-1.10.6_szip_mpich/lib'
./configure  
</pre>

Build the CCR code with `make`, install the CCR library with
`make install`, and then run tests with `make check`.

# REFERENCES

Hartnett, E, Zender, C.S., Fisher, W., Heimbigner, D., Lei, H.,
Gerheiser, K., Curtis, B. (2021), Quantization and Next-Generation
Zlib Compression for Fully Backward-Compatible, Faster, and More
Effective Data Compression in NetCDF Files,
https://www.researchgate.net/publication/357001251_Quantization_and_Next-Generation_Zlib_Compression_for_Fully_Backward-Compatible_Faster_and_More_Effective_Data_Compression_in_NetCDF_Files.

Hartnett, E, Zender, C.S., Fisher, W., Heimbigner, D., Lei, H.,
Gerheiser, K., Curtis, B. (2021), Presentation - Quantization and Next-Generation
Zlib Compression for Fully Backward-Compatible, Faster, and More
Effective Data Compression in NetCDF Files,
https://www.researchgate.net/publication/357000984_Quantization_and_Next-Generation_Zlib_Compression_for_Fully_Backward-Compatible_Faster_and_More_Effective_Data_Compression_in_NetCDF_Files

Hartnett, E, Zender, C.S. (2021), Poster - ADDITIONAL NETCDF
COMPRESSION OPTIONS WITH THE COMMUNITY CODEC REPOSITORY, AMS Annual
Meeting,
https://www.researchgate.net/publication/347726899_Poster_-_ADDITIONAL_NETCDF_COMPRESSION_OPTIONS_WITH_THE_COMMUNITY_CODEC_REPOSITORY.

Kouznetsov, R. (2021), A note on precision-preserving compression of
scientific data, Geosci. Model Dev., 14(1), 377-389,
https://doi.org/10.5194/gmd-14-377-2021

Delaunay, X., A. Courtois, and F. Gouillon (2019), Evaluation of
lossless and lossy algorithms for the compression of scientific
datasets in netCDF-4 or HDF5 files, Geosci. Model Dev., 12(9),
4099-4113, doi:10.5194/gmd-2018-250, retrieved on Sep 21, 2020 from
https://www.researchgate.net/publication/335987647_Evaluation_of_lossless_and_lossy_algorithms_for_the_compression_of_scientific_datasets_in_netCDF-4_or_HDF5_files.

Zender, C. S. (2016), Bit Grooming: Statistically accurate
precision-preserving quantization with compression, evaluated in the
netCDF Operators (NCO, v4.4.8+), Geosci. Model Dev., 9, 3199-3211,
doi:10.5194/gmd-9-3199-2016 Retrieved on Sep 21, 2020 from
https://www.researchgate.net/publication/301575383_Bit_Grooming_Statistically_accurate_precision-preserving_quantization_with_compression_evaluated_in_the_netCDF_Operators_NCO_v448.

Hartnett, E. (2011), netCDF-4/HDF5 File Format,
https://earthdata.nasa.gov/files/ESDS-RFC-022v1.pdf

# ACKNOWLEDGEMENTS

The Community Codec Repository (CCR) used in this work is supported by
the National Science Foundation under Grant No. OAC-2004993.
