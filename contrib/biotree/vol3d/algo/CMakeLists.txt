# created by K. Kang for holding all the algorithm based on 3d volume data 
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )

set(vol3d_algo_src
    vol3d_radius_detection.h                      vol3d_radius_detection.cxx
    vol3d_gaussian_filtering_proc.hxx             vol3d_gaussian_filtering_proc.h
)

aux_source_directory(Templates vol3d_algo_src)
add_library(vol3d_algo ${vol3d_algo_src})

target_link_libraries(vol3d_algo vcl vbl vol3d bsta)

add_subdirectory(io)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif(BUILD_EXAMPLES)

