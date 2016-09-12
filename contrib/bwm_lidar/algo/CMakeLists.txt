include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
#include_directories( ${VXL_BRL_INCLUDE_DIR}/bmvl )
#include_directories( ${lems_SOURCE_DIR}/brcv/mvg )
include_directories( ${lems_SOURCE_DIR} )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/contrib/bwm_lidar/algo )

set( bwm_lidar_algo_sources
  all_includes.h
  flimap_reader.cxx  flimap_reader.h
  get_connected_components.cxx  get_connected_components.h
  lidar_labeler.cxx  lidar_labeler.h
  set_operations.cxx  set_operations.h
  world_modeler.cxx  world_modeler.h

  bwm_lidar_mesh.cxx   bwm_lidar_mesh.h
  bwm_lidar_algo.cxx   bwm_lidar_algo.h
)

aux_source_directory(Templates bwm_lidar_algo_sources)

add_library(bwm_lidar_algo  ${bwm_lidar_algo_sources})

target_link_libraries(bwm_lidar_algo vil_algo imesh dbmsh3d dbmsh3d_algo)
# vbl vpdfl vpgl vpgl_algo


