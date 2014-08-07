IF (VXL_FOUND)
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_SOURCE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/vehicleseg/pro )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/vehicleseg/vis)

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/Gregistration )


INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )

SET( dborl_multi_frame_fg_edge_sources
  dborl_multi_frame_fg_edge_main.cxx 
  dbrl_edge_point_tangent_bg_model_process_2.h
  dbrl_edge_point_tangent_bg_model_process_2.cxx
)

ADD_EXECUTABLE (dborl_multi_frame_fg_edge ${dborl_multi_frame_fg_edge_sources} )
TARGET_LINK_LIBRARIES (dborl_multi_frame_fg_edge dborl_algo dbdet_pro bpro1  vidpro1  bvis1 georegistration_pro dbgl_algo brip  georegistration_vis dbvidl_pro  dbvrl_pro dbgl_algo  vsl dbil_pro dbbgm_vis dbbgm_pro dbdet_pro dbinfo_pro dbinfo_vis dbacm_pro dbsol_pro dbsta_algo dbdet_vis dbru_pro)

ENDIF(VXL_FOUND)
