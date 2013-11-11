INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1)
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

SET( dbccl_pro_sources
   dbccl_aerial_tracker_process.cxx      dbccl_aerial_tracker_process.h
   dbccl_manual_camera_process.cxx       dbccl_manual_camera_process.h
   dbccl_camera_estimator_process.cxx    dbccl_camera_estimator_process.h
)

ADD_LIBRARY(dbccl_pro ${dbccl_pro_sources})

TARGET_LINK_LIBRARIES(dbccl_pro vidpro1 dbccl dvpgl_pro)
