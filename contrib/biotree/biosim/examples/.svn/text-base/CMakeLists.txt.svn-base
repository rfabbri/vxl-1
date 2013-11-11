IF(SIMVOLEON_FOUND)
  INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/basic ${lems_SOURCE_DIR}/contrib/biotree/proc ${VXL_GEL_INCLUDE_DIR})
  INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
  INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/contrib/biotree/resc )
  INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/contrib/biotree/biosim )
  INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
  INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )


ADD_EXECUTABLE(volume_vis_example biosim_volume_vis.cxx )
TARGET_LINK_LIBRARIES(volume_vis_example biosim vgl vil vsl vgui vol3d bgui3d dbsta2 biov)


ENDIF(SIMVOLEON_FOUND )

