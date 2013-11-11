# created by K. Kang for holding all the algorithm based on 3d volume data 
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )

SET(vol3d_algo_src
    vol3d_radius_detection.h                      vol3d_radius_detection.cxx
    vol3d_gaussian_filtering_proc.txx             vol3d_gaussian_filtering_proc.h
)

AUX_SOURCE_DIRECTORY(Templates vol3d_algo_src)
ADD_LIBRARY(vol3d_algo ${vol3d_algo_src})

TARGET_LINK_LIBRARIES(vol3d_algo vcl vbl vol3d bsta)

SUBDIRS(io)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

IF(BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF(BUILD_EXAMPLES)

