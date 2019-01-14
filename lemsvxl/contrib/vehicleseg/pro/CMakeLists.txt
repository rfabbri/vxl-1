

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${LEMS_SOURCE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/contrib/vehicleseg )

set( vehicleseg_pro_sources
         )


#aux_source_directory(Templates vehicleseg_pro_sources)

#add_library(vehicleseg_pro  ${vehicleseg_pro_sources})

#target_link_libraries(vehicleseg_pro dbbgm vil vil_io  vimt bpro1 dbvrl dbvrl_pro dbbgm_pro dbgl_algo bsta dbsta dbsta_algo bsol)

