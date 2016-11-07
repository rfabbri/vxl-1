cmake_minimum_required(VERSION 2.6)

if(VXL_FOUND)
  #include the standard vxl stuff

  project(scale_prob)
  set(VXL_PROVIDE_STANDARD_OPTIONS 1)
  include(${VXL_CMAKE_DIR}/UseVXL.cmake)
  
  add_executable( exec scale_prob_map_main.cxx)
  
  target_link_libraries(exec vnl vil vul)
    
endif(VXL_FOUND)
