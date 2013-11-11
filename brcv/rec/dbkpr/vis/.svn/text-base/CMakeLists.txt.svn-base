# This is brcv/rec/dbkpr/vis/CMakeLists.txt
# Brown Keypoint Recognition Visualization Library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

IF (VXL_VGUI_FOUND)

SET(dbkpr_vis_sources
   dbkpr_keypoint_tools.cxx        dbkpr_keypoint_tools.h
)

IF (VXL_BGUI3D_FOUND)

ADD_DEFINITIONS ( -DCOIN_NOT_DLL )
INCLUDE_DIRECTORIES( ${VXL_COIN3D_INCLUDE_DIR} )

SET(dbkpr_vis_sources ${dbkpr_vis_sources}
   dbkpr_corr3d_displayer.cxx      dbkpr_corr3d_displayer.h
)
ENDIF (VXL_BGUI3D_FOUND)


#AUX_SOURCE_DIRECTORY(Templates dbkpr_vis_sources)

ADD_LIBRARY(dbkpr_vis ${dbkpr_vis_sources})

TARGET_LINK_LIBRARIES(dbkpr_vis dbkpr_pro dbdet_vis)

# Add the bgui3d library if bgui3d is built is available for 3D rendering
IF (VXL_BGUI3D_FOUND)
  TARGET_LINK_LIBRARIES(dbkpr_vis bgui3d)
ENDIF (VXL_BGUI3D_FOUND)


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)


ENDIF (VXL_VGUI_FOUND)
