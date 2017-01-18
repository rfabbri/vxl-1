if (VXL_FOUND)
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_OXL_INCLUDE_DIR} )
include_directories(${VXL_MUL_INCLUDE_DIR} )
include_directories(${VXL_SOURCE_DIR} )

include_directories( ${lems_SOURCE_DIR}/contrib/vehicleseg/pro )
include_directories( ${lems_SOURCE_DIR}/contrib/vehicleseg/vis)

include_directories( ${lems_SOURCE_DIR}/contrib/Gregistration )


include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/bmvgd )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/basic )

include_directories( ${lems_SOURCE_DIR}/contrib )

set( dborl_multi_frame_fg_edge_sources
  dborl_multi_frame_fg_edge_main.cxx 
  dbrl_edge_point_tangent_bg_model_process_2.h
  dbrl_edge_point_tangent_bg_model_process_2.cxx
)

add_executable (dborl_multi_frame_fg_edge ${dborl_multi_frame_fg_edge_sources} )
target_link_libraries (dborl_multi_frame_fg_edge dborl_algo dbdet_pro bpro1  vidpro1  bvis1 georegistration_pro bgld_algo brip  georegistration_vis dbvidl_pro  dbvrl_pro bgld_algo  vsl bild_pro dbbgm_vis dbbgm_pro dbdet_pro dbinfo_pro dbinfo_vis dbacm_pro bsold_pro dbsta_algo dbdet_vis dbru_pro)

endif(VXL_FOUND)
