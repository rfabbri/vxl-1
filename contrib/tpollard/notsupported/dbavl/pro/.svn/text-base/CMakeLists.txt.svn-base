INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1)
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

SET( dbavl_pro_sources
   dbavl_track_process.cxx               dbavl_track_process.h
   dbavl_manual_camera_process.cxx       dbavl_manual_camera_process.h
   dbavl_camera_estimator_process.cxx    dbavl_camera_estimator_process.h
)


ADD_LIBRARY(dbavl_pro ${dbavl_pro_sources})

TARGET_LINK_LIBRARIES(dbavl_pro vidpro1 dbavl )



