CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(VXL_FOUND)

  PROJECT(bmvrec)
  SET(VXL_PROVIDE_STANDARD_OPTIONS 1)
  INCLUDE(${VXL_CMAKE_DIR}/UseVXL.cmake)

  INCLUDE_DIRECTORIES( /home/brandon/vxl/contrib/brl/bseg )
  INCLUDE_DIRECTORIES( /home/brandon/vxl/contrib/gel )
  INCLUDE_DIRECTORIES( /home/brandon/vxl/contrib/brl/bbas )

  SET(bmvrec_sources bmvrec_cmd.cxx bmvrec_functions.h     bmvrec_functions.cxx)

  ADD_EXECUTABLE(bmvrec_cmd ${bmvrec_sources})

  TARGET_LINK_LIBRARIES(bmvrec_cmd bsta bsta_algo brip vnl vil vul)

ENDIF(VXL_FOUND)
