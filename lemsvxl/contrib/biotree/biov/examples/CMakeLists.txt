# library for storing / loading 3d image 
include_directories(${LEMS_SOURCE_DIR}/basic ${LEMS_SOURCE_DIR}/brcv/mvg ${biotree_SOURCE_DIR}/resc)

if(SIMVOLEON_FOUND)

  include_directories( ${VXL_MUL_INCLUDE_DIR} )
  add_executable(biov_main biov_main.cxx)
  target_link_libraries(biov_main bgui3d biov vil3d vil3d_io vil3d_algo slice)

endif(SIMVOLEON_FOUND)
