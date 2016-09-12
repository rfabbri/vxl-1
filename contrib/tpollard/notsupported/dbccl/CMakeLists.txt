include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bmvl )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_RPL_INCLUDE_DIR})
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${lems_SOURCE_DIR}/basic )

add_library( dbccl
  dbccl_aerial_tracker.h       dbccl_aerial_tracker.cxx
  dbccl_camera_estimator.h     dbccl_camera_estimator.cxx  
  dbccl_manual_camera.h        dbccl_manual_camera.cxx
  dbccl_fast_mi.h              dbccl_fast_mi.cxx )


target_link_libraries( dbccl vcl vgl vnl vil vidl1 vgl_algo vil_algo vpgl dvpgl_algo brct )
set( DBCCL_INCLUDE_DIR ${dbccl_SOURCE_DIR} )


subdirs(tests)
subdirs(pro)
