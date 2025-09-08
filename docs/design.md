# CCR Design Document

## Overview
CCR (Climate Compression and Reduction) is a library that provides various compression algorithms optimized for climate and scientific data. This document outlines the current architecture and planned changes.

## Current Architecture (v1.x)
The library supports multiple compression algorithms through a unified API:
- **BZIP2**: General-purpose compression
- **LZ4**: Fast compression with good performance
- **GRANULARBR**: Granular bit rounding for lossy compression (being removed in v2.0.0)
- **BITROUND**: Bit rounding compression (being removed in v2.0.0)
- **ZStandard**: High-performance compression (being removed in v2.0.0)
- **Bit Grooming**: Lossy compression for floating-point data (being removed in v2.0.0)

## v2.0.0 Architecture Changes

### Algorithm Removal
As part of v2.0.0 Sprint 1, the following algorithms are being removed:
- **ZStandard**: Complete removal of all ZStandard compression functionality
- **Bit Grooming**: Complete removal of all Bit Grooming compression functionality
- **GRANULARBR**: Complete removal of all GRANULARBR compression functionality
- **BITROUND**: Complete removal of all BITROUND compression functionality

### Remaining Algorithm Set
After v2.0.0 Sprint 1, CCR will support only:
- **BZIP2**: General-purpose compression with good compression ratios
- **LZ4**: Fast compression with excellent performance characteristics

### Component Architecture

#### Core Library (`src/ccr.c`)
- Main API implementation
- Algorithm dispatch and management
- Error handling and validation
- Memory management

#### HDF5 Plugins (`hdf5_plugins/`)
- Individual plugin directories for each algorithm
- Standardized plugin interface
- Example code and tests for each plugin

#### Language Bindings
- **C Interface**: Primary interface in `include/ccr.h`
- **Fortran Interface**: Fortran 90 bindings in `fsrc/ccr.F90`

#### Build System
- **Autotools**: Primary build system using autoconf/automake
- **Modular Configuration**: Each plugin can be enabled/disabled
- **Dependency Management**: Automatic detection of required libraries

### API Changes in v2.0.0
The removal of ZStandard, Bit Grooming, GRANULARBR, and BITROUND will result in:
- Removal of algorithm-specific constants and enums for all removed algorithms
- Removal of algorithm-specific function calls
- Significantly simplified configuration options
- Reduced library dependencies
- Streamlined API focused on two core algorithms

### Testing Framework
- **Unit Tests**: Algorithm-specific tests in `test/`
- **HDF5 Integration Tests**: Plugin tests in `test_h5/`
- **Fortran Tests**: Fortran interface tests in `ftest/`
- **Performance Benchmarks**: Benchmark suite for performance validation

### Build and CI/CD
- **GitHub Actions**: Automated testing on multiple platforms
- **Multiple Build Configurations**: Standard, ASAN, parallel builds
- **Cross-platform Support**: Linux, macOS support

## Migration Guide for v2.0.0
Users migrating from v1.x to v2.0.0 should:
1. Remove any references to ZStandard compression in their code
2. Remove any references to Bit Grooming compression in their code
3. Remove any references to GRANULARBR compression in their code
4. Remove any references to BITROUND compression in their code
5. Update build configurations to remove dependencies for removed algorithms
6. Migrate to BZIP2 for general-purpose compression with good ratios
7. Migrate to LZ4 for fast compression with excellent performance
8. Update any HDF5 filter configurations to use only BZIP2 or LZ4
9. Review and update any algorithm-specific configuration parameters

## Future Roadmap
- **v2.1.0**: Add CMake build system support
- **Future versions**: Potential addition of new compression algorithms based on community needs
