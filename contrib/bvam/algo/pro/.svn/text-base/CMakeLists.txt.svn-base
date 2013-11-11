# This is contrib/bvam/algo/pro/CMakeLists.txt
# Bvam's Algorithm Process Library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR})

SET(bvam_algo_pro_sources
    bvam_ekf_camera_optimize_process.h  bvam_ekf_camera_optimize_process.cxx
    bvam_init_ekf_camera_optimizer_process.h  bvam_init_ekf_camera_optimizer_process.cxx
)

AUX_SOURCE_DIRECTORY(Templates bvam_algo_pro_sources)

ADD_LIBRARY(bvam_algo_pro ${bvam_algo_pro_sources})

TARGET_LINK_LIBRARIES(bvam_algo_pro bvam bprb vpgl vpgl_io vil vil_io vnl vnl_io bgeo vgl vgl_algo vsol brip brdb)





