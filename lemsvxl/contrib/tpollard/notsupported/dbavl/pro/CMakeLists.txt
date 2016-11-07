include_directories( ${lems_SOURCE_DIR}/breye1)
include_directories( ${VXL_GEL_INCLUDE_DIR} )

set( dbavl_pro_sources
   dbavl_track_process.cxx               dbavl_track_process.h
   dbavl_manual_camera_process.cxx       dbavl_manual_camera_process.h
   dbavl_camera_estimator_process.cxx    dbavl_camera_estimator_process.h
)


add_library(dbavl_pro ${dbavl_pro_sources})

target_link_libraries(dbavl_pro vidpro1 dbavl )



