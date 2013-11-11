

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/vehicleseg )

SET( vehicleseg_pro_sources
         )


#AUX_SOURCE_DIRECTORY(Templates vehicleseg_pro_sources)

#ADD_LIBRARY(vehicleseg_pro  ${vehicleseg_pro_sources})

#TARGET_LINK_LIBRARIES(vehicleseg_pro dbbgm vil vil_io  vimt bpro1 dbvrl dbvrl_pro dbbgm_pro dbgl_algo bsta dbsta dbsta_algo bsol)

