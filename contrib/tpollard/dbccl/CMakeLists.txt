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
  dbccl_manual_camera.h        dbccl_manual_camera.cxx )


target_link_libraries( dbccl vcl vgl vnl vil vidl1 vgl_algo vil_algo vpgl )
set( DBCCL_INCLUDE_DIR ${dbccl_SOURCE_DIR} )

add_executable ( dbccl_manual_app dbccl_manual_app.cxx )
#add_executable ( dbccl_manual_app_rewrite dbccl_manual_app_rewrite.cxx )
add_executable ( dbccl_refine_pose_app dbccl_refine_pose_app.cxx )
target_link_libraries( dbccl_manual_app vcl vgl vnl vpgl vpgl_algo vul )
#target_link_libraries( dbccl_manual_app_rewrite vcl vgl vnl vpgl vpgl_algo vul )
target_link_libraries( dbccl_refine_pose_app vcl vgl vnl vpgl vpgl_algo vul )

add_subdirectory(tests)
add_subdirectory(pro)
