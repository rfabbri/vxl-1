include_directories( ${LEMS_SOURCE_DIR}/breye1)
include_directories( ${VXL_GEL_INCLUDE_DIR} )

set( dbccl_pro_sources
   dbccl_manual_camera_process.cxx       dbccl_manual_camera_process.h
)

add_library(dbccl_pro ${dbccl_pro_sources})

target_link_libraries(dbccl_pro vidpro1 dbccl dvpgl_pro)
