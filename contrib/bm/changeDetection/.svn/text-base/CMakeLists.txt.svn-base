CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(VXL_FOUND)
  #include the standard vxl stuff

  PROJECT(scale_prob)
  SET(VXL_PROVIDE_STANDARD_OPTIONS 1)
  INCLUDE(${VXL_CMAKE_DIR}/UseVXL.cmake)
  
  ADD_EXECUTABLE( exec scale_prob_map_main.cxx)
  
  TARGET_LINK_LIBRARIES(exec vnl vil vul)
    
ENDIF(VXL_FOUND)
