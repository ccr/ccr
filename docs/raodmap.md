# CCR Roadmap

## v2.1.0: Add CMake Build System
### Sprint 1: Add CMake Support
- Add CMake build system as alternative to autotools
- Maintain backward compatibility with existing autotools build
- Update CI/CD pipelines to test both build systems
- Create CMake configuration for all remaining algorithms (BZIP2, LZ4)
- Update documentation with CMake build instructions

## v2.0.0: Major Algorithm Cleanup - Simplify to BZIP2 and LZ4 Only
This is a major change which removed those features that have been merged into netcdf-c. 

### Sprint 1: Remove Compression Algorithms Supported by netcdf-c
**Goal**: Remove all compression algorithms except BZIP2 and LZ4

**Algorithms to Remove**:
- **ZStandard**: High-performance compression (complete removal)
- **Bit Grooming**: Lossy compression for floating-point data (complete removal)  
- **GRANULARBR**: Granular bit rounding compression (complete removal)
- **BITROUND**: Bit rounding compression (complete removal)

**Scope of Work**:
- Remove entire HDF5 plugin directories for removed algorithms
- Clean up core library code (`src/ccr.c`, `include/ccr.h`)
- Remove Fortran interface support for removed algorithms
- Delete all test files and test references for removed algorithms
- Update build system configuration (autotools)
- Update CI/CD workflows
- Update documentation and examples
- In all CI files in .github/workflows all "actions/cache@v2" need to be replaced with "actions/cache@v4"

**Remaining Algorithms After Sprint 1**:
- **BZIP2**: General-purpose compression with good compression ratios
- **LZ4**: Fast compression with excellent performance characteristics

### Sprint 2: LZ4 Enhancement and Validation
**Goal**: Ensure LZ4 is fully optimized and well-tested as a primary algorithm

**Scope of Work**:
- Validate LZ4 implementation is complete and optimized
- Enhance LZ4 test coverage
- Performance benchmarking against removed algorithms
- Documentation updates highlighting LZ4 as recommended fast compression
- Migration guide for users moving from removed algorithms to LZ4

**Deliverables**:
- Comprehensive LZ4 performance benchmarks
- Migration documentation
- Updated API documentation focusing on BZIP2/LZ4 usage patterns

