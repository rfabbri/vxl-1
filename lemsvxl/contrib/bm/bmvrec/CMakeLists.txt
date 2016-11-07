cmake_minimum_required(VERSION 2.6)

if(VXL_FOUND)

  project(bmvrec)
  set(VXL_PROVIDE_STANDARD_OPTIONS 1)
  include(${VXL_CMAKE_DIR}/UseVXL.cmake)

  include_directories( /home/brandon/vxl/contrib/brl/bseg )
  include_directories( /home/brandon/vxl/contrib/gel )
  include_directories( /home/brandon/vxl/contrib/brl/bbas )

  set(bmvrec_sources bmvrec_cmd.cxx bmvrec_functions.h     bmvrec_functions.cxx)

  add_executable(bmvrec_cmd ${bmvrec_sources})

  target_link_libraries(bmvrec_cmd bsta bsta_algo brip vnl vil vul)

endif(VXL_FOUND)
