# This is contrib/bvam/algo/pro/CMakeLists.txt
# Bvam's Algorithm Process Library

include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bseg )
include_directories( ${lems_SOURCE_DIR}/contrib )
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
include_directories( ${VXL_MUL_INCLUDE_DIR})

set(bvam_algo_pro_sources
    bvam_ekf_camera_optimize_process.h  bvam_ekf_camera_optimize_process.cxx
    bvam_init_ekf_camera_optimizer_process.h  bvam_init_ekf_camera_optimizer_process.cxx
)

aux_source_directory(Templates bvam_algo_pro_sources)

add_library(bvam_algo_pro ${bvam_algo_pro_sources})

target_link_libraries(bvam_algo_pro bvam bprb vpgl vpgl_io vil vil_io vnl vnl_io bgeo vgl vgl_algo vsol brip brdb)





