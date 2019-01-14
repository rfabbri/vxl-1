# This is contrib/dec/breg3d/CMakeLists.txt
# 3-d video registration using the bvxm library

include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${LEMS_SOURCE_DIR}/contrib )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_MUL_INCLUDE_DIR})

set(breg3d_sources
    breg3d_ekf_camera_optimizer.h         breg3d_ekf_camera_optimizer.cxx
    breg3d_ekf_camera_optimizer_state.h   breg3d_ekf_camera_optimizer_state.cxx
    breg3d_homography_generator.h
    breg3d_lm_direct_homography_generator.h  breg3d_lm_direct_homography_generator.cxx
    breg3d_gdbicp_homography_generator.h  breg3d_gdbicp_homography_generator.cxx
)

#aux_source_directory(Templates breg3d_sources)

add_library(breg3d ${breg3d_sources})

target_link_libraries(breg3d bvxm vpgl vpgl_algo vil vnl bgeo vgl vgl_algo vsol brip brdb vimt dbvrl)

add_subdirectory(pro)
add_subdirectory(tests)



