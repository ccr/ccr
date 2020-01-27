  ! This is a test program for the CCR LZ4 compression filter for
  ! netCDF. This started as an example nc4_pres_temp_4D_wr.f90 from
  ! the netcdf-fortran project.

  ! Ed Hartnett 1/23/20

program ftst_ccr_lz4
  use netcdf
  use ccr
  implicit none

  ! This is the name of the data file we will create.
  character (len = *), parameter :: FILE_NAME = "ftst_ccr_lz4.nc"
  integer :: ncid

  ! We are writing 4D data.
  integer, parameter :: NDIMS = 4, NRECS = 2
  integer, parameter :: NLVLS = 20, NLATS = 60, NLONS = 120
  character (len = *), parameter :: LVL_NAME = "level"
  character (len = *), parameter :: LAT_NAME = "latitude"
  character (len = *), parameter :: LON_NAME = "longitude"
  character (len = *), parameter :: REC_NAME = "time"
  integer :: lvl_dimid, lon_dimid, lat_dimid, rec_dimid

  ! The start and count arrays will tell the netCDF library where to
  ! write our data.
  integer :: start(NDIMS), count(NDIMS)

  ! These program variables hold the latitudes and longitudes.
  real :: lats(NLATS), lons(NLONS)
  integer :: lon_varid, lat_varid
  integer, parameter :: COMPRESSION_LEVEL = 3
  integer :: lz4p, levelp

  ! We will create two netCDF variables, one each for temperature and
  ! pressure fields.
  character (len = *), parameter :: PRES_NAME="pressure"
  character (len = *), parameter :: TEMP_NAME="temperature"
  integer :: pres_varid, temp_varid
  integer :: dimids(NDIMS)

  ! Program variables to hold the data we will write out. We will only
  ! need enough space to hold one timestep of data; one record.
  real, dimension(:,:,:), allocatable :: pres_out
  real, dimension(:,:,:), allocatable :: temp_out
  real, parameter :: SAMPLE_PRESSURE = 900.0
  real, parameter :: SAMPLE_TEMP = 9.0

  ! Use these to construct some latitude and longitude data for this
  ! example.
  real, parameter :: START_LAT = 25.0, START_LON = -125.0

  ! Loop indices
  integer :: lvl, lat, lon, rec, i

  ! Program variables to hold the data we will read in. We will only
  ! need enough space to hold one timestep of data; one record.
  ! Allocate memory for data.
  real, dimension(:,:,:), allocatable :: pres_in
  real, dimension(:,:,:), allocatable :: temp_in

  print *, '*** Testing CCR Fortran library...'

  ! Allocate memory.
  allocate(pres_out(NLONS, NLATS, NLVLS))
  allocate(temp_out(NLONS, NLATS, NLVLS))

  i = 0
  do lvl = 1, NLVLS
     do lat = 1, NLATS
        do lon = 1, NLONS
           pres_out(lon, lat, lvl) = SAMPLE_PRESSURE + i
           temp_out(lon, lat, lvl) = SAMPLE_TEMP + i
           i = i + 1
        end do
     end do
  end do

  ! Initialize the CCR.
  call check( nf90_initialize_ccr() )

  ! Create the file.
  call check( nf90_create(FILE_NAME, NF90_NETCDF4, ncid) )

  ! Define the dimensions.
  call check( nf90_def_dim(ncid, LVL_NAME, NLVLS, lvl_dimid) )
  call check( nf90_def_dim(ncid, LAT_NAME, NLATS, lat_dimid) )
  call check( nf90_def_dim(ncid, LON_NAME, NLONS, lon_dimid) )
  call check( nf90_def_dim(ncid, REC_NAME, NF90_UNLIMITED, rec_dimid) )

  ! Define the netCDF variables for the pressure and temperature data.
  dimids = (/ lon_dimid, lat_dimid, lvl_dimid, rec_dimid /)
  call check( nf90_def_var(ncid, PRES_NAME, NF90_REAL, dimids, pres_varid) )
  call check( nf90_def_var_lz4(ncid, pres_varid, COMPRESSION_LEVEL) )
  call check( nf90_def_var(ncid, TEMP_NAME, NF90_REAL, dimids, temp_varid) )
  call check( nf90_def_var_lz4(ncid, temp_varid, COMPRESSION_LEVEL) )

  ! Check the compression settings.
  call check( nf90_inq_var_lz4(ncid, pres_varid, lz4p, levelp) )
  if (levelp .ne. COMPRESSION_LEVEL) stop 2
  if (lz4p .ne. 1) stop 2
  levelp = 0
  lz4p = 0
  call check( nf90_inq_var_lz4(ncid, temp_varid, lz4p, levelp) )
  if (levelp .ne. COMPRESSION_LEVEL) stop 2
  if (lz4p .ne. 1) stop 2

  ! End define mode.
  call check( nf90_enddef(ncid) )

  ! Write the pretend data.
  count = (/ NLONS, NLATS, NLVLS, 1 /)
  start = (/ 1, 1, 1, 1 /)
  do rec = 1, NRECS
     start(4) = rec
     call check( nf90_put_var(ncid, pres_varid, pres_out, start = start, &
                              count = count) )
     call check( nf90_put_var(ncid, temp_varid, temp_out, start = start, &
                              count = count) )
  end do

  ! Close the file.
  call check( nf90_close(ncid) )

  ! Allocate memory.
  allocate(pres_in(NLONS, NLATS, NLVLS))
  allocate(temp_in(NLONS, NLATS, NLVLS))

  ! Re-open the file.
  call check( nf90_open(FILE_NAME, nf90_nowrite, ncid) )

  ! Get the varids of the pressure and temperature netCDF variables.
  call check( nf90_inq_varid(ncid, PRES_NAME, pres_varid) )
  call check( nf90_inq_varid(ncid, TEMP_NAME, temp_varid) )

  ! Check the compression settings.
  call check( nf90_inq_var_lz4(ncid, pres_varid, lz4p, levelp) )
  if (levelp .ne. COMPRESSION_LEVEL) stop 2
  if (lz4p .ne. 1) stop 2
  levelp = 0
  lz4p = 0
  call check( nf90_inq_var_lz4(ncid, temp_varid, lz4p, levelp) )
  if (levelp .ne. COMPRESSION_LEVEL) stop 2
  if (lz4p .ne. 1) stop 2

  ! Read the data and check it.
  count = (/ NLONS, NLATS, NLVLS, 1 /)
  start = (/ 1, 1, 1, 1 /)
  do rec = 1, NRECS
     start(4) = rec
     call check( nf90_get_var(ncid, pres_varid, pres_in, start = start, &
                              count = count) )
     call check( nf90_get_var(ncid, temp_varid, temp_in, start, count) )

     i = 0
     do lvl = 1, NLVLS
        do lat = 1, NLATS
           do lon = 1, NLONS
              if (pres_in(lon, lat, lvl) /= SAMPLE_PRESSURE + i) stop 2
              if (temp_in(lon, lat, lvl) /= SAMPLE_TEMP + i) stop 2
              i = i + 1
           end do
        end do
     end do
     ! next record
  end do

  ! Close the file.
  call check( nf90_close(ncid) )

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
end program ftst_ccr_lz4
