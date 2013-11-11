INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bmvl )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_RPL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

ADD_LIBRARY( dbavl
  dbavl_tracker.h              dbavl_tracker.cxx
  dbavl_camera_estimator.h     dbavl_camera_estimator.cxx  
  dbavl_manual_camera.h        dbavl_manual_camera.cxx )


TARGET_LINK_LIBRARIES( dbavl vcl vgl vnl vil vidl1 vgl_algo vil_algo vpgl dvpgl_algo brct )
SET( DBAVL_INCLUDE_DIR ${dbavl_SOURCE_DIR} )


SUBDIRS(tests)
SUBDIRS(pro)
