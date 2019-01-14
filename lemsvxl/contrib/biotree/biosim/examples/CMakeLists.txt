if(SIMVOLEON_FOUND)
  include_directories(${LEMS_SOURCE_DIR}/basic ${LEMS_SOURCE_DIR}/contrib/biotree/proc ${VXL_GEL_INCLUDE_DIR})
  include_directories( ${LEMS_SOURCE_DIR}/db3p )
  include_directories(${LEMS_SOURCE_DIR}/contrib/biotree/resc )
  include_directories(${LEMS_SOURCE_DIR}/contrib/biotree/biosim )
  include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
  include_directories( ${VXL_MUL_INCLUDE_DIR} )


add_executable(volume_vis_example biosim_volume_vis.cxx )
target_link_libraries(volume_vis_example biosim vgl vil vsl vgui vol3d bgui3d dbsta2 biov)


endif(SIMVOLEON_FOUND )

