  ! This is the Fortran wrapper for the CCR library.

  ! Ed Hartnett 1/25/20

module ccr

  interface
     function nc_initialize_ccr() bind(c)
       use iso_c_binding
     end function nc_initialize_ccr
  end interface

  ! interface
  !    function glm_read_event_arrays(ncid, nevent, event_id, time_offset, &
  !         lat, lon, energy, parent_group_id) bind(c)
  !      use iso_c_binding
  !      integer(C_INT), value :: ncid
  !      integer(C_SIZE_T), intent(INOUT) :: nevent
  !      integer(C_INT), intent(INOUT) :: event_id(*)
  !      real(C_FLOAT), intent(INOUT) :: time_offset(*)
  !      real(C_FLOAT), intent(INOUT) :: lat(*)
  !      real(C_FLOAT), intent(INOUT) :: lon(*)
  !      real(C_FLOAT), intent(INOUT) :: energy(*)
  !      integer(C_INT), intent(INOUT) :: parent_group_id(*)
  !    end function glm_read_event_arrays
  ! end interface

  ! interface
  !    function glm_read_group_arrays(ncid, ngroup, time_offset, &
  !         lat, lon, energy, area, parent_flash_id, quality_flag) bind(c)
  !      use iso_c_binding
  !      integer(C_INT), value :: ncid
  !      integer(C_SIZE_T), intent(INOUT) :: ngroup
  !      real(C_FLOAT), intent(INOUT) :: time_offset(*)
  !      real(C_FLOAT), intent(INOUT) :: lat(*)
  !      real(C_FLOAT), intent(INOUT) :: lon(*)
  !      real(C_FLOAT), intent(INOUT) :: energy(*)
  !      real(C_FLOAT), intent(INOUT) :: area(*)
  !      integer(C_INT), intent(INOUT) :: parent_flash_id(*)
  !      integer(C_SHORT), intent(INOUT) :: quality_flag(*)
  !    end function glm_read_group_arrays
  ! end interface

  contains
  !   function fglm_read_dims(ncid, nevent, ngroup, nflash) result(status)
  !     use iso_c_binding
  !     implicit none
  !     integer, intent(in) :: ncid
  !     integer, intent(out) :: nevent, ngroup, nflash
  !     integer*8 :: nevent8, ngroup8, nflash8
  !     integer :: status
  !     integer(C_INT) :: cstatus

  !     status = glm_read_dims(ncid, nevent8, ngroup8, nflash8)
  !     nevent = nevent8
  !     ngroup = ngroup8
  !     nflash = nflash8
  !   end function fglm_read_dims
    function nf90_initialize_ccr() result(status)
      use iso_c_binding
      implicit none
      integer :: status
      integer(C_INT) :: cstatus
      status = nc_initialize_ccr()
    end function nf90_initialize_ccr
end module ccr
