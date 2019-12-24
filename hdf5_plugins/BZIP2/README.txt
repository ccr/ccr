Building this filter/example requires knowledge of the hdf5 and the bzip2
installation.

For autotools configure, you must supply these using
    --with-hdf5 and --with-bz2lib.
Example (in the build folder):
    ../configure --with-hdf5=/temp/hdf5 --with-bz2lib=/temp/bz2

For CMake, you must supply the location of the cmake configuration files 
    in environment variables.
    In addition, CMake options "BUILD_EXAMPLES" and "BUILD_TESTING" must
    be set "ON" in order to build the example and run the tests.
Example:
    set(ENV{HDF5_DIR} "/temp/hdf5/share/cmake/hdf5")
    set(ENV{BZ2_DIR} "/temp/bz2/share/cmake/bz2")
    set(ENV{LD_LIBRARY_PATH} "/temp/bz2/lib:/temp/hdf5/lib")
    set(ADD_BUILD_OPTIONS "-DBUILD_EXAMPLES:BOOL=ON -DBUILD_TESTING:BOOL=ON")

    For non-cmake built hdf5 or bzip2, use the location of the include/lib
    folders:
    set(ENV{HDF5_ROOT} "/temp/hdf5")
    set(ENV{BZ2_ROOT} "/temp/bz2")
