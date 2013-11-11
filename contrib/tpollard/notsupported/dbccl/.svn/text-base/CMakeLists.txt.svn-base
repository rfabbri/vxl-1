INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bmvl )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

ADD_LIBRARY( dbccl
  dbccl_aerial_tracker.h       dbccl_aerial_tracker.cxx
  dbccl_camera_estimator.h     dbccl_camera_estimator.cxx  
  dbccl_manual_camera.h        dbccl_manual_camera.cxx
  dbccl_fast_mi.h              dbccl_fast_mi.cxx )


TARGET_LINK_LIBRARIES( dbccl vcl vgl vnl vil vidl1 vgl_algo vil_algo vpgl dvpgl_algo brct )
SET( DBCCL_INCLUDE_DIR ${dbccl_SOURCE_DIR} )


SUBDIRS(tests)
SUBDIRS(pro)
