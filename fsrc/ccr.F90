  ! This is the Fortran wrapper for the CCR library.

  ! Ed Hartnett 1/25/20

module ccr

  interface
     function nc_initialize_ccr() bind(c)
       use iso_c_binding
     end function nc_initialize_ccr
  end interface

  interface
     function nc_def_var_bzip2(ncid, varid, level) bind(c)
       use iso_c_binding
       integer(C_INT), value :: ncid, varid, level
     end function nc_def_var_bzip2
  end interface

  interface
     function nc_inq_var_bzip2(ncid, varid, bzip2p, levelp) bind(c)
       use iso_c_binding
       integer(C_INT), value :: ncid, varid
       integer(C_INT), intent(inout):: bzip2p, levelp
     end function nc_inq_var_bzip2
  end interface

  interface
     function nc_def_var_lz4(ncid, varid, level) bind(c)
       use iso_c_binding
       integer(C_INT), value :: ncid, varid, level
     end function nc_def_var_lz4
  end interface

  interface
     function nc_inq_var_lz4(ncid, varid, lz4p, levelp) bind(c)
       use iso_c_binding
       integer(C_INT), value :: ncid, varid
       integer(C_INT), intent(inout):: lz4p, levelp
     end function nc_inq_var_lz4
  end interface

  contains
    function nf90_initialize_ccr() result(status)
      use iso_c_binding
      implicit none
      integer :: status
      integer(C_INT) :: cstatus
      status = nc_initialize_ccr()
    end function nf90_initialize_ccr

    function nf90_def_var_bzip2(ncid, varid, level) result(status)
      use iso_c_binding
      implicit none
      integer, intent(in) :: ncid, varid, level
      integer :: status

      ! C varids start at 0, fortran at 1.
      status = nc_def_var_bzip2(ncid, varid - 1, level)
    end function nf90_def_var_bzip2

    function nf90_inq_var_bzip2(ncid, varid, bzip2p, levelp) result(status)
      use iso_c_binding
      implicit none
      integer, intent(in) :: ncid, varid
      integer, intent(inout) :: bzip2p, levelp
      integer :: status

      ! C varids start at 0, fortran at 1.
      status = nc_inq_var_bzip2(ncid, varid - 1, bzip2p, levelp)
    end function nf90_inq_var_bzip2

    function nf90_def_var_lz4(ncid, varid, level) result(status)
      use iso_c_binding
      implicit none
      integer, intent(in) :: ncid, varid, level
      integer :: status

      ! C varids start at 0, fortran at 1.
      status = nc_def_var_lz4(ncid, varid - 1, level)
    end function nf90_def_var_lz4

    function nf90_inq_var_lz4(ncid, varid, lz4p, levelp) result(status)
      use iso_c_binding
      implicit none
      integer, intent(in) :: ncid, varid
      integer, intent(inout) :: lz4p, levelp
      integer :: status

      ! C varids start at 0, fortran at 1.
      status = nc_inq_var_lz4(ncid, varid - 1, lz4p, levelp)
    end function nf90_inq_var_lz4

end module ccr
