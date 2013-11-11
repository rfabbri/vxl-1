# This is contrib/bvam/algo/CMakeLists.txt
# Bvam's Algorithm Library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR})

SET(bvam_algo_sources
    bvam_ekf_camera_optimizer.h         bvam_ekf_camera_optimizer.cxx
    bvam_ekf_camera_optimizer_state.h   bvam_ekf_camera_optimizer_state.cxx
)

#AUX_SOURCE_DIRECTORY(Templates bvam_algo_sources)

ADD_LIBRARY(bvam_algo ${bvam_algo_sources})

TARGET_LINK_LIBRARIES(bvam_algo bvam vpgl vpgl_algo vil vnl bgeo vgl vgl_algo vsol brip brdb vimt dbvrl)

SUBDIRS(pro)
SUBDIRS(tests)



