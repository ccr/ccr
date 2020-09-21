!> @file
!!
!! This is the Fortran wrapper for the CCR library.
!!
!! @author Ed Hartnett
!! @date 1/25/20

module ccr

  !> Interface to initialization function.
  interface
     function nc_initialize_ccr() bind(c)
       use iso_c_binding
     end function nc_initialize_ccr
  end interface

  !> Interface to C function to set BZIP2 compression.
  interface
     function nc_def_var_bzip2(ncid, varid, level) bind(c)
       use iso_c_binding
       integer(C_INT), value :: ncid, varid, level
     end function nc_def_var_bzip2
  end interface

  !> Interface to C function to inquire about BZIP2 compression.
  interface
     function nc_inq_var_bzip2(ncid, varid, bzip2p, levelp) bind(c)
       use iso_c_binding
       integer(C_INT), value :: ncid, varid
       integer(C_INT), intent(inout):: bzip2p, levelp
     end function nc_inq_var_bzip2
  end interface

  !> Interface to C function to set LZ4 compression.
  interface
     function nc_def_var_lz4(ncid, varid, level) bind(c)
       use iso_c_binding
       integer(C_INT), value :: ncid, varid, level
     end function nc_def_var_lz4
  end interface

  !> Interface to C function to inquire about LZ4 compression.
  interface
     function nc_inq_var_lz4(ncid, varid, lz4p, levelp) bind(c)
       use iso_c_binding
       integer(C_INT), value :: ncid, varid
       integer(C_INT), intent(inout):: lz4p, levelp
     end function nc_inq_var_lz4
  end interface

  !> Interface to C function to set BitGroom quantization.
  interface
     function nc_def_var_bitgroom(ncid, varid, nsd) bind(c)
       use iso_c_binding
       integer(C_INT), value :: ncid, varid, nsd
     end function nc_def_var_bitgroom
  end interface

  !> Interface to C function to inquire about BitGroom quantization.
  interface
     function nc_inq_var_bitgroom(ncid, varid, bitgroomp, nsdp) bind(c)
       use iso_c_binding
       integer(C_INT), value :: ncid, varid
       integer(C_INT), intent(inout):: bitgroomp, nsdp
     end function nc_inq_var_bitgroom
  end interface
  
  !> Interface to C function to set Zstandard compression.
  interface
     function nc_def_var_zstandard(ncid, varid, level) bind(c)
       use iso_c_binding
       integer(C_INT), value :: ncid, varid, level
     end function nc_def_var_zstandard
  end interface

  !> Interface to C function to inquire about Zstandard compression.
  interface
     function nc_inq_var_zstandard(ncid, varid, zstandardp, levelp) bind(c)
       use iso_c_binding
       integer(C_INT), value :: ncid, varid
       integer(C_INT), intent(inout):: zstandardp, levelp
     end function nc_inq_var_zstandard
  end interface

contains
  !> Initialize the CCR filters.
  function nf90_initialize_ccr() result(status)
    use iso_c_binding
    implicit none
    integer :: status
    integer(C_INT) :: cstatus
    status = nc_initialize_ccr()
  end function nf90_initialize_ccr

  !> Set BZIP2 compression for a variable.
  !!
  !! @param ncid File or group ID.
  !! @param varid Variable ID.
  !! @param level The compression level.
  !!
  !! @return 0 for sucess, error code otherwise.
  function nf90_def_var_bzip2(ncid, varid, level) result(status)
    use iso_c_binding
    implicit none
    integer, intent(in) :: ncid, varid, level
    integer :: status

    ! C varids start at 0, fortran at 1.
    status = nc_def_var_bzip2(ncid, varid - 1, level)
  end function nf90_def_var_bzip2

  !> Inquire about BZIP2 compression for a variable.
  !!
  !! @param ncid File or group ID.
  !! @param varid Variable ID.
  !! @param bzip2p Pointer that gets 1 if BZIP2 is in use, 0
  !! otherwise. Ignored if NULL.
  !! @param levelp Pointer that gets compression level, if BZIP2 is in
  !! use. Ignored if NULL.
  !!
  !! @return 0 for sucess, error code otherwise.
  function nf90_inq_var_bzip2(ncid, varid, bzip2p, levelp) result(status)
    use iso_c_binding
    implicit none
    integer, intent(in) :: ncid, varid
    integer, intent(inout) :: bzip2p, levelp
    integer :: status

    ! C varids start at 0, fortran at 1.
    status = nc_inq_var_bzip2(ncid, varid - 1, bzip2p, levelp)
  end function nf90_inq_var_bzip2

  !> Set LZ4 compression for a variable.
  !!
  !! @param ncid File or group ID.
  !! @param varid Variable ID.
  !! @param level The compression level.
  !!
  !! @return 0 for sucess, error code otherwise.
  function nf90_def_var_lz4(ncid, varid, level) result(status)
    use iso_c_binding
    implicit none
    integer, intent(in) :: ncid, varid, level
    integer :: status

    ! C varids start at 0, fortran at 1.
    status = nc_def_var_lz4(ncid, varid - 1, level)
  end function nf90_def_var_lz4

  !> Inquire about LZ4 compression for a variable.
  !!
  !! @param ncid File or group ID.
  !! @param varid Variable ID.
  !! @param lz4p Pointer that gets 1 if LZ4 is in use, 0
  !! otherwise. Ignored if NULL.
  !! @param levelp Pointer that gets compression level, if LZ4 is in
  !! use. Ignored if NULL.
  !!
  !! @return 0 for sucess, error code otherwise.
  function nf90_inq_var_lz4(ncid, varid, lz4p, levelp) result(status)
    use iso_c_binding
    implicit none
    integer, intent(in) :: ncid, varid
    integer, intent(inout) :: lz4p, levelp
    integer :: status

    ! C varids start at 0, fortran at 1.
    status = nc_inq_var_lz4(ncid, varid - 1, lz4p, levelp)
  end function nf90_inq_var_lz4

  !> Set BitGroom quantization for a variable.
  !!
  !! @param ncid File or group ID.
  !! @param varid Variable ID.
  !! @param nsd Number of significant digits to retain. Allowed single- and
  !! double-precision NSDs are 1-7 and 1-15, respectively. (Default is 3).
  !!
  !! @return 0 for sucess, error code otherwise.
  function nf90_def_var_bitgroom(ncid, varid, nsd) result(status)
    use iso_c_binding
    implicit none
    integer, intent(in) :: ncid, varid, nsd
    integer :: status

    ! C varids start at 0, fortran at 1.
    status = nc_def_var_bitgroom(ncid, varid - 1, nsd)
  end function nf90_def_var_bitgroom

  !> Inquire about BitGroom quantization for a variable.
  !!
  !! @param ncid File or group ID.
  !! @param varid Variable ID.
  !! @param bitgroomp Pointer that gets 1 if BitGroom is in use, 0
  !! otherwise. Ignored if NULL.
  !! @param nsdp Pointer that gets number of significant digits,
  !! if BitGroom is in use. Ignored if NULL.
  !!
  !! @return 0 for sucess, error code otherwise.
  function nf90_inq_var_bitgroom(ncid, varid, bitgroomp, nsdp) result(status)
    use iso_c_binding
    implicit none
    integer, intent(in) :: ncid, varid
    integer, intent(inout) :: bitgroomp, nsdp
    integer :: status

    ! C varids start at 0, fortran at 1.
    status = nc_inq_var_bitgroom(ncid, varid - 1, bitgroomp, nsdp)
  end function nf90_inq_var_bitgroom

  !> Set Zstandard compression for a variable.
  !!
  !! @param ncid File or group ID.
  !! @param varid Variable ID.
  !! @param level The compression level.
  !!
  !! @return 0 for sucess, error code otherwise.
  function nf90_def_var_zstandard(ncid, varid, level) result(status)
    use iso_c_binding
    implicit none
    integer, intent(in) :: ncid, varid, level
    integer :: status

    ! C varids start at 0, fortran at 1.
    status = nc_def_var_zstandard(ncid, varid - 1, level)
  end function nf90_def_var_zstandard

  !> Inquire about Zstandard compression for a variable.
  !!
  !! @param ncid File or group ID.
  !! @param varid Variable ID.
  !! @param zstandardp Pointer that gets 1 if Zstandard is in use, 0
  !! otherwise. Ignored if NULL.
  !! @param levelp Pointer that gets compression level, if Zstandard is in
  !! use. Ignored if NULL.
  !!
  !! @return 0 for sucess, error code otherwise.
  function nf90_inq_var_zstandard(ncid, varid, zstandardp, levelp) result(status)
    use iso_c_binding
    implicit none
    integer, intent(in) :: ncid, varid
    integer, intent(inout) :: zstandardp, levelp
    integer :: status

    ! C varids start at 0, fortran at 1.
    status = nc_inq_var_zstandard(ncid, varid - 1, zstandardp, levelp)
  end function nf90_inq_var_zstandard

end module ccr
