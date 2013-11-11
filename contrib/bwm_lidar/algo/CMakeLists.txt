INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
#INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bmvl )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/bwm_lidar/algo )

SET( bwm_lidar_algo_sources
  all_includes.h
  flimap_reader.cxx  flimap_reader.h
  get_connected_components.cxx  get_connected_components.h
  lidar_labeler.cxx  lidar_labeler.h
  set_operations.cxx  set_operations.h
  world_modeler.cxx  world_modeler.h

  bwm_lidar_mesh.cxx   bwm_lidar_mesh.h
  bwm_lidar_algo.cxx   bwm_lidar_algo.h
)

AUX_SOURCE_DIRECTORY(Templates bwm_lidar_algo_sources)

ADD_LIBRARY(bwm_lidar_algo  ${bwm_lidar_algo_sources})

TARGET_LINK_LIBRARIES(bwm_lidar_algo vil_algo imesh dbmsh3d dbmsh3d_algo)
# vbl vpdfl vpgl vpgl_algo


