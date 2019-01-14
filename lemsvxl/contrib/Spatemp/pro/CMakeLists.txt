

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${LEMS_SOURCE_DIR}/contrib )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/breye )
set( spatemp_pro_sources
spatemp_display_edge_maps_process.h     spatemp_display_edge_maps_process.cxx
spatemp_form_temporal_bundles_process.h spatemp_form_temporal_bundles_process.cxx
vidpro1_load_video_edgemap_process.h vidpro1_load_video_edgemap_process.cxx
# storage
dbdet_temporal_map_storage.h dbdet_temporal_map_storage.cxx dbdet_temporal_map_storage_sptr.h
spatemp_forward_beta_process.h  spatemp_forward_beta_process.cxx
spatemp_backward_beta_process.h spatemp_backward_beta_process.cxx
 )


aux_source_directory(Templates spatemp_pro_sources)
add_library( spatemp_pro ${spatemp_pro_sources})

target_link_libraries( spatemp_pro spatemp_algo bpro1 vbl vil_io  dbdet_pro dbbgm_pro dbsta_algo dbbgm sdet)
