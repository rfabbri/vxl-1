INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/biotree/xmvg)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic)
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

ADD_EXECUTABLE(bioproj_proc_example bioproj_proc_example.cxx )
TARGET_LINK_LIBRARIES(bioproj_proc_example bioproj)

ADD_EXECUTABLE(bioproj_mem_proc_example bioproj_mem_proc_example.cxx )
TARGET_LINK_LIBRARIES(bioproj_mem_proc_example bioproj slice vcsl bscs)

ADD_EXECUTABLE(bioproj_ridge_detection_example bioproj_ridge_detection_example.cxx )
TARGET_LINK_LIBRARIES(bioproj_ridge_detection_example bioproj dbil3d_algo vil3d_algo biodet slice)
