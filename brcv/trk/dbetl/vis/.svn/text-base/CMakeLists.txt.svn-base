# This is brcv/trk/dbetl/vis/CMakeLists.txt
# Brown Epipolar Tracking Visualization library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )

IF (VXL_VGUI_FOUND)

SET(dbetl_vis_sources
   dbetl_episeg_tools.cxx        dbetl_episeg_tools.h
)

# Add the following files if bgui3d is available for 3D rendering
IF (VXL_BGUI3D_FOUND)
SET(dbetl_vis_sources ${dbetl_vis_sources}
   dbetl_track_displayer.cxx     dbetl_track_displayer.h
)
ENDIF (VXL_BGUI3D_FOUND)

ADD_DEFINITIONS ( -DCOIN_NOT_DLL )
INCLUDE_DIRECTORIES( ${VXL_COIN3D_INCLUDE_DIR} )

#AUX_SOURCE_DIRECTORY(Templates dbetl_vis_sources)

ADD_LIBRARY(dbetl_vis ${dbetl_vis_sources})

TARGET_LINK_LIBRARIES(dbetl_vis dbetl_pro dbecl_vis)

# Add the bgui3d library if bgui3d is built is available for 3D rendering
IF (VXL_BGUI3D_FOUND)
  TARGET_LINK_LIBRARIES(dbetl_vis bgui3d)
ENDIF (VXL_BGUI3D_FOUND)


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)


ENDIF (VXL_VGUI_FOUND)
