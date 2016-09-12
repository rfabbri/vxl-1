

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${lems_SOURCE_DIR} )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/contrib/ )
include_directories( ${lems_SOURCE_DIR}/breye )
include_directories( ${lems_SOURCE_DIR}/basic )


#set( itrack_pro_sources
#)

#aux_source_directory(Templates itrack_pro_sources)

#add_library(itrack_pro ${itrack_pro_sources})

#target_link_libraries(itrack_pro dbinfo bpro vil_io brip bsol)
