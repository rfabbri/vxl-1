
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/breye )
include_directories( ${lems_SOURCE_DIR}/contrib )
set( spatemp_vis_sources
dbdet_temporal_map_displayer.h dbdet_temporal_map_displayer.cxx
dbdet_temporal_map_tableau.h dbdet_temporal_map_tableau.cxx dbdet_temporal_map_tableau_sptr.h
 )


aux_source_directory(Templates spatemp_vis_sources)
add_library( spatemp_vis ${spatemp_vis_sources})

target_link_libraries( spatemp_vis  bpro1  vbl vil_io)
