  ! This is a test program for the CCR BZIP2 compression filter for netCDF.

  ! Ed Hartnett 1/23/20

program ftst_ccr_bzip2
  use netcdf
  implicit none
  integer ncid
  character (len = *), parameter :: filename = &
       '../test/OR_GLM-L2-LCFA_G17_s20192692359400_e20192700000000_c20192700000028.nc'
  integer :: nevent, ngroup, nflash

  print *, '*** Testing ncglm Fortran library...'
!   call check(nf90_open(filename, 0, ncid))
!   call check(fglm_read_dims(ncid, nevent, ngroup, nflash))
!   if (nevent .ne. 4578) stop 2;
!   if (ngroup .ne. 1609) stop 3;
!   if (nflash .ne. 123) stop 4;

! !  || ngroup != 1609 || nflash != 123) ERR;
!   call check(nf90_close(ncid))
  print *, '*** SUCCESS!!'

contains
  ! Internal subroutine - checks error status after each netcdf, prints out text message each time
  !   an error code is returned.
  subroutine check(status)
    integer, intent ( in) :: status

    if(status /= nf90_noerr) then
      print *, trim(nf90_strerror(status))
      stop 2
    end if
  end subroutine check
end program ftst_ccr_bzip2
