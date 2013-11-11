# contrib/vehicleseg/gui/CMakeLists.txt



INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/vehicleseg/pro )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/vehicleseg/vis)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/Gregistration )


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )

SET(georegister_match_db_sources
  match_db_rpm.cxx
)
ADD_EXECUTABLE( georegister_match_db ${georegister_match_db_sources})
TARGET_LINK_LIBRARIES( georegister_match_db georegistration_pro vul)

SET(georegister_match_db_affine_sources
  match_db_rpm_affine.cxx
)
ADD_EXECUTABLE( georegister_match_db_affine ${georegister_match_db_affine_sources})
TARGET_LINK_LIBRARIES( georegister_match_db_affine georegistration_pro vul)


SET( georegister_exe_sources
     multiple_frame_polygon.h multiple_frame_polygon.cxx)
ADD_LIBRARY( georegister_exe SHARED ${georegister_exe_sources})
TARGET_LINK_LIBRARIES( georegister_exe dbsta2_algo dbbgm_pro vil dbdet dbinfo_pro vnl_algo  georegister dbgl_algo )


