cmake_minimum_required(VERSION 2.6)

if(VXL_FOUND)
  # include the standard vxl stuff

  project(vehicleRecognition)
  set(VXL_PROVIDE_STANDARD_OPTIONS 1)
  include(${VXL_CMAKE_DIR}/UseVXL.cmake)

  include_directories( /home/brandon/vxl/contrib/brl/bseg )
  include_directories( /home/brandon/vxl/contrib/gel )
  include_directories( /home/brandon/vxl/contrib/brl/bbas )

  set(sources driver.cxx bmvr_functions.h
                         bmvr_functions.cxx )

  add_executable(exec ${sources})

  target_link_libraries(exec bsta bsta_algo brip vnl vil vul)

endif(VXL_FOUND)
