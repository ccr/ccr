  ! This is a test program for the CCR Granular BitRound quantization filter for
  ! netCDF. This started as an example nc4_pres_temp_4D_wr.f90 from
  ! the netcdf-fortran project.

  ! Ed Hartnett 1/23/20, Charlie Zender 10/17/21

program ftst_ccr_granularbr
  use netcdf
  use ccr
  implicit none

  ! This is the name of the data file we will create.
  character (len = *), parameter :: FILE_NAME = "ftst_ccr_granularbr.nc"
  integer :: ncid

  ! We are writing 4D data.
  integer, parameter :: NDIMS = 4, NRECS = 2
  integer, parameter :: NLVLS = 20, NLATS = 30, NLONS = 60
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
  integer, parameter :: QUANTIZATION_NSD = 3
  integer :: granularbrp, nsdp

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
  real, dimension(:,:,:), allocatable :: pres_tst
  real, dimension(:,:,:), allocatable :: temp_tst

  ! Program variables to constrain quantization success check
  real :: scale

  print *, '*** Testing CCR Fortran library...'

  ! Allocate memory.
  allocate(pres_out(NLONS, NLATS, NLVLS))
  allocate(temp_out(NLONS, NLATS, NLVLS))
  allocate(pres_tst(NLONS, NLATS, NLVLS))
  allocate(temp_tst(NLONS, NLATS, NLVLS))

  i = 0
  do lvl = 1, NLVLS
     do lat = 1, NLATS
        do lon = 1, NLONS
           pres_out(lon, lat, lvl) = SAMPLE_PRESSURE + i
           pres_tst(lon, lat, lvl) = SAMPLE_PRESSURE + i
           temp_out(lon, lat, lvl) = SAMPLE_TEMP + i
           temp_tst(lon, lat, lvl) = SAMPLE_TEMP + i
           i = i + 1
        end do
     end do
  end do

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
  call check( nf90_def_var_granularbr(ncid, pres_varid, QUANTIZATION_NSD) )
  call check( nf90_def_var(ncid, TEMP_NAME, NF90_REAL, dimids, temp_varid) )
  call check( nf90_def_var_granularbr(ncid, temp_varid, QUANTIZATION_NSD) )

  ! Check the quantization settings.
  call check( nf90_inq_var_granularbr(ncid, pres_varid, granularbrp, nsdp) )
  if (nsdp .ne. QUANTIZATION_NSD) stop 2
  if (granularbrp .ne. 1) stop 2
  nsdp = 0
  granularbrp = 0
  call check( nf90_inq_var_granularbr(ncid, temp_varid, granularbrp, nsdp) )
  if (nsdp .ne. QUANTIZATION_NSD) stop 2
  if (granularbrp .ne. 1) stop 2

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

  ! Check the quantization settings.
  call check( nf90_inq_var_granularbr(ncid, pres_varid, granularbrp, nsdp) )
  if (nsdp .ne. QUANTIZATION_NSD) stop 2
  if (granularbrp .ne. 1) stop 2
  nsdp = 0
  granularbrp = 0
  call check( nf90_inq_var_granularbr(ncid, temp_varid, granularbrp, nsdp) )
  if (nsdp .ne. QUANTIZATION_NSD) stop 2
  if (granularbrp .ne. 1) stop 2

  ! Read the data and check it.
  count = (/ NLONS, NLATS, NLVLS, 1 /)
  start = (/ 1, 1, 1, 1 /)
  do rec = 1, NRECS
     start(4) = rec
     call check( nf90_get_var(ncid, pres_varid, pres_in, start = start, &
                              count = count) )
     call check( nf90_get_var(ncid, temp_varid, temp_in, start, count) )

     i = 0
     scale = 10.0**nsdp
     do lvl = 1, NLVLS
        do lat = 1, NLATS
           do lon = 1, NLONS
              ! Check the data. Quantization alter data, so do not check for equality :) */
              pres_tst(lon,lat,lvl)=nint(scale*pres_out(lon,lat,lvl))/scale
              if (abs(pres_in(lon,lat,lvl)-pres_tst(lon,lat,lvl)) > abs(5.0*pres_out(lon,lat,lvl)/scale)) then
                 write (6,'(a10,f15.8,a4,f15.8,a11)') 'pres_in = ',pres_in(lon,lat,lvl),' !~ ', &
                      pres_tst(lon,lat,lvl),' = pres_tst'
                 stop 2
              end if ! pres_in
              temp_tst(lon,lat,lvl)=nint(scale*temp_out(lon,lat,lvl))/scale
              if (abs(temp_in(lon,lat,lvl)-temp_tst(lon,lat,lvl)) > abs(5.0*temp_out(lon,lat,lvl)/scale)) then
                 write (6,'(a10,f15.8,a4,f15.8,a11)') 'temp_in = ',temp_in(lon,lat,lvl),' !~ ', &
                      temp_tst(lon,lat,lvl),' = temp_tst'
                 stop 2
              end if ! temp_in
              i = i + 1
           end do
        end do
     end do
     ! next record
  end do

  ! Close the file.
  call check( nf90_close(ncid) )

  deallocate(pres_in)
  deallocate(temp_in)
  deallocate(pres_out)
  deallocate(temp_out)
  deallocate(pres_tst)
  deallocate(temp_tst)

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
end program ftst_ccr_granularbr
