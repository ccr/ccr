cmake_minimum_required (VERSION 3.10)
project (H5BZ2_SRC C)

#-----------------------------------------------------------------------------
# Apply Definitions to compiler in this directory and below
#-----------------------------------------------------------------------------
add_definitions (${HDF5_EXTRA_C_FLAGS})

#-----------------------------------------------------------------------------
# Setup include Directories
#-----------------------------------------------------------------------------
set(H5BZ2_INCLUDE_DIRS ${H5BZ2_INCLUDE_DIRS}
    ${HDF5_INCLUDE_DIR} ${H5BZ2_BINARY_DIR}
    ${H5BZ2_SRC_SOURCE_DIR}
    ${BZ2_INCLUDE_DIRS}
)

#-----------------------------------------------------------------------------
# Define H5BZ2 Library
#-----------------------------------------------------------------------------
set (H5BZ2_SRCS
    ${H5BZ2_SRC_SOURCE_DIR}/H5Zbzip2.c
)

add_library (${H5BZ2_LIB_TARGET} MODULE ${H5BZ2_SRCS})
if (DISABLE_H5BZ2_ENCODER)
  set_target_properties(${H5BZ2_LIB_TARGET} PROPERTIES COMPILE_DEFINITIONS "FILTER_DECODE_ONLY")
endif ()
target_include_directories(${H5BZ2_LIB_TARGET} PRIVATE ${H5BZ2_INCLUDE_DIRS})
target_link_libraries (${H5BZ2_LIB_TARGET} PRIVATE ${LINK_LIBS})
set_global_variable (H5BZ2_LIBRARIES_TO_EXPORT ${H5BZ2_LIB_TARGET})
HDF_SET_LIB_VERSIONS ("H5BZ2" ${H5BZ2_LIB_TARGET} ${H5BZ2_LIB_NAME} MODULE)

if (BUILD_TESTING)
  #-----------------------------------------------------------------------------
  # Copy plugin library to a plugins folder
  #-----------------------------------------------------------------------------

  # make plugins dir
  file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/plugins")
  add_custom_command (
      TARGET     ${H5BZ2_LIB_TARGET}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy_if_different
        "$<TARGET_FILE:${H5BZ2_LIB_TARGET}>"
        "${CMAKE_BINARY_DIR}/plugins/$<TARGET_FILE_NAME:${H5BZ2_LIB_TARGET}>"
  )
endif ()

#-----------------------------------------------------------------------------
# Add Target(s) to CMake Install for import into other projects
#-----------------------------------------------------------------------------
if (H5BZ2_EXPORTED_TARGETS)
  install (
      TARGETS
          ${H5BZ2_LIB_TARGET}
      EXPORT
          ${H5BZ2_EXPORTED_TARGETS}
      LIBRARY DESTINATION ${H5BZ2_INSTALL_LIB_DIR} COMPONENT libraries
      ARCHIVE DESTINATION ${H5BZ2_INSTALL_LIB_DIR} COMPONENT libraries
      RUNTIME DESTINATION ${H5BZ2_INSTALL_BIN_DIR} COMPONENT libraries
  )
endif ()
