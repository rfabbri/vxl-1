# This is contrib/bvam/algo/CMakeLists.txt
# Bvam's Algorithm Library

include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${lems_SOURCE_DIR}/contrib )
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_MUL_INCLUDE_DIR})

set(bvam_algo_sources
    bvam_ekf_camera_optimizer.h         bvam_ekf_camera_optimizer.cxx
    bvam_ekf_camera_optimizer_state.h   bvam_ekf_camera_optimizer_state.cxx
)

#aux_source_directory(Templates bvam_algo_sources)

add_library(bvam_algo ${bvam_algo_sources})

target_link_libraries(bvam_algo bvam vpgl vpgl_algo vil vnl bgeo vgl vgl_algo vsol brip brdb vimt dbvrl)

subdirs(pro)
subdirs(tests)



