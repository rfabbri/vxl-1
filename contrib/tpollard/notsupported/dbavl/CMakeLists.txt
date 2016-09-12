include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bmvl )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_RPL_INCLUDE_DIR})
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${lems_SOURCE_DIR}/basic )

add_library( dbavl
  dbavl_tracker.h              dbavl_tracker.cxx
  dbavl_camera_estimator.h     dbavl_camera_estimator.cxx  
  dbavl_manual_camera.h        dbavl_manual_camera.cxx )


target_link_libraries( dbavl vcl vgl vnl vil vidl1 vgl_algo vil_algo vpgl dvpgl_algo brct )
set( DBAVL_INCLUDE_DIR ${dbavl_SOURCE_DIR} )


add_subdirectory(tests)
add_subdirectory(pro)
