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
  dbccl_manual_camera.h        dbccl_manual_camera.cxx )


TARGET_LINK_LIBRARIES( dbccl vcl vgl vnl vil vidl1 vgl_algo vil_algo vpgl )
SET( DBCCL_INCLUDE_DIR ${dbccl_SOURCE_DIR} )

ADD_EXECUTABLE ( dbccl_manual_app dbccl_manual_app.cxx )
#ADD_EXECUTABLE ( dbccl_manual_app_rewrite dbccl_manual_app_rewrite.cxx )
ADD_EXECUTABLE ( dbccl_refine_pose_app dbccl_refine_pose_app.cxx )
TARGET_LINK_LIBRARIES( dbccl_manual_app vcl vgl vnl vpgl vpgl_algo vul )
#TARGET_LINK_LIBRARIES( dbccl_manual_app_rewrite vcl vgl vnl vpgl vpgl_algo vul )
TARGET_LINK_LIBRARIES( dbccl_refine_pose_app vcl vgl vnl vpgl vpgl_algo vul )

SUBDIRS(tests)
SUBDIRS(pro)
