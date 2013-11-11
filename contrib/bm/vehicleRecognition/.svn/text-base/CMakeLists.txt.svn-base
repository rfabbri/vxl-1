CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(VXL_FOUND)
  # include the standard vxl stuff

  PROJECT(vehicleRecognition)
  SET(VXL_PROVIDE_STANDARD_OPTIONS 1)
  INCLUDE(${VXL_CMAKE_DIR}/UseVXL.cmake)

  INCLUDE_DIRECTORIES( /home/brandon/vxl/contrib/brl/bseg )
  INCLUDE_DIRECTORIES( /home/brandon/vxl/contrib/gel )
  INCLUDE_DIRECTORIES( /home/brandon/vxl/contrib/brl/bbas )

  SET(sources driver.cxx bmvr_functions.h
                         bmvr_functions.cxx )

  ADD_EXECUTABLE(exec ${sources})

  TARGET_LINK_LIBRARIES(exec bsta bsta_algo brip vnl vil vul)

ENDIF(VXL_FOUND)
