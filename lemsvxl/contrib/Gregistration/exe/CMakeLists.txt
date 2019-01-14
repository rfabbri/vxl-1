# contrib/vehicleseg/gui/CMakeLists.txt



include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )


include_directories( ${LEMS_SOURCE_DIR}/contrib/vehicleseg/pro )
include_directories( ${LEMS_SOURCE_DIR}/contrib/vehicleseg/vis)

include_directories( ${LEMS_SOURCE_DIR}/contrib/Gregistration )


include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/basic )

include_directories( ${LEMS_SOURCE_DIR}/contrib )

set(georegister_match_db_sources
  match_db_rpm.cxx
)
add_executable( georegister_match_db ${georegister_match_db_sources})
target_link_libraries( georegister_match_db georegistration_pro vul)

set(georegister_match_db_affine_sources
  match_db_rpm_affine.cxx
)
add_executable( georegister_match_db_affine ${georegister_match_db_affine_sources})
target_link_libraries( georegister_match_db_affine georegistration_pro vul)


set( georegister_exe_sources
     multiple_frame_polygon.h multiple_frame_polygon.cxx)
add_library( georegister_exe SHARED ${georegister_exe_sources})
target_link_libraries( georegister_exe dbsta2_algo dbbgm_pro vil dbdet dbinfo_pro vnl_algo  georegister dbgl_algo )


