include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bmvl )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${LEMS_SOURCE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/contrib/bwm_lidar/pro )
include_directories( ${LEMS_SOURCE_DIR}/contrib/bwm_lidar/algo )

set( bwm_lidar_pro_sources  
  bwm_lidar_cmdpara.cxx  bwm_lidar_cmdpara.h
)

aux_source_directory(Templates bwm_lidar_pro_sources)

add_library(bwm_lidar_pro  ${bwm_lidar_pro_sources})

target_link_libraries(bwm_lidar_pro bwm_lidar_algo vbl vpdfl vpgl vpgl_algo)
