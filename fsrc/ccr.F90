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

end module ccr
