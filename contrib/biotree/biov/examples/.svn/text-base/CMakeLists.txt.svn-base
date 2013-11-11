# library for storing / loading 3d image 
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/basic ${lems_SOURCE_DIR}/brcv/mvg ${biotree_SOURCE_DIR}/resc)

IF(SIMVOLEON_FOUND)

  INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
  ADD_EXECUTABLE(biov_main biov_main.cxx)
  TARGET_LINK_LIBRARIES(biov_main bgui3d biov vil3d vil3d_io vil3d_algo slice)

ENDIF(SIMVOLEON_FOUND)
