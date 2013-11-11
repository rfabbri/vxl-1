

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye )
SET( spatemp_pro_sources
spatemp_display_edge_maps_process.h     spatemp_display_edge_maps_process.cxx
spatemp_form_temporal_bundles_process.h spatemp_form_temporal_bundles_process.cxx
vidpro1_load_video_edgemap_process.h vidpro1_load_video_edgemap_process.cxx
# storage
dbdet_temporal_map_storage.h dbdet_temporal_map_storage.cxx dbdet_temporal_map_storage_sptr.h
spatemp_forward_beta_process.h  spatemp_forward_beta_process.cxx
spatemp_backward_beta_process.h spatemp_backward_beta_process.cxx
 )


AUX_SOURCE_DIRECTORY(Templates spatemp_pro_sources)
ADD_LIBRARY( spatemp_pro ${spatemp_pro_sources})

TARGET_LINK_LIBRARIES( spatemp_pro spatemp_algo bpro1 vbl vil_io  dbdet_pro dbbgm_pro dbsta_algo dbbgm sdet)
