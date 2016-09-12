include_directories( ${lems_SOURCE_DIR}/contrib/biotree/xmvg)
include_directories( ${lems_SOURCE_DIR}/basic)
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

add_executable(bioproj_proc_example bioproj_proc_example.cxx )
target_link_libraries(bioproj_proc_example bioproj)

add_executable(bioproj_mem_proc_example bioproj_mem_proc_example.cxx )
target_link_libraries(bioproj_mem_proc_example bioproj slice vcsl bscs)

add_executable(bioproj_ridge_detection_example bioproj_ridge_detection_example.cxx )
target_link_libraries(bioproj_ridge_detection_example bioproj dbil3d_algo vil3d_algo biodet slice)
