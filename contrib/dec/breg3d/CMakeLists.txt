# This is contrib/dec/breg3d/CMakeLists.txt
# 3-d video registration using the bvxm library

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR})

SET(breg3d_sources
    breg3d_ekf_camera_optimizer.h         breg3d_ekf_camera_optimizer.cxx
    breg3d_ekf_camera_optimizer_state.h   breg3d_ekf_camera_optimizer_state.cxx
    breg3d_homography_generator.h
    breg3d_lm_direct_homography_generator.h  breg3d_lm_direct_homography_generator.cxx
    breg3d_gdbicp_homography_generator.h  breg3d_gdbicp_homography_generator.cxx
)

#AUX_SOURCE_DIRECTORY(Templates breg3d_sources)

ADD_LIBRARY(breg3d ${breg3d_sources})

TARGET_LINK_LIBRARIES(breg3d bvxm vpgl vpgl_algo vil vnl bgeo vgl vgl_algo vsol brip brdb vimt dbvrl)

SUBDIRS(pro)
SUBDIRS(tests)



