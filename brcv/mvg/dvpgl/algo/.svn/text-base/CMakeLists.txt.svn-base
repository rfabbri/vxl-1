# brcv/mvg/dvpgl/algo/CMakeLists.txt
# Photogrametry Algorithms Library.

INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR}/mrc)
INCLUDE_DIRECTORIES(${VXL_RPL_INCLUDE_DIR})

# Code in this library has been promoted to sourceforge
# Recreate the library if needed

SET( dvpgl_algo_sources
  dvpgl_triangulation.h dvpgl_triangulation.cxx 
  dvpgl_distmap_bundle_adjust.h dvpgl_distmap_bundle_adjust.cxx 
  # Under development: dvpgl_camera_compute_ransac.h dvpgl_camera_compute_ransac.cxx 
)

ADD_LIBRARY(dvpgl_algo ${dvpgl_algo_sources})


TARGET_LINK_LIBRARIES(dvpgl_algo vpgl_algo triangulation_kanatani)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

SUBDIRS(triangulation_kanatani)

SUBDIRS(cmd)

SUBDIRS(pro)

