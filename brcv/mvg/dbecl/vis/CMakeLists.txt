# This is brcv/mvg/dbecl/vis/CMakeLists.txt
# Brown Epipolar Curve Visualization library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

IF (VXL_VGUI_FOUND)

SET(dbecl_vis_sources
   dbecl_episeg_soview2D.cxx     dbecl_episeg_soview2D.h
   dbecl_episeg_displayer.cxx    dbecl_episeg_displayer.h
   dbecl_episeg_tools.cxx        dbecl_episeg_tools.h
)

#AUX_SOURCE_DIRECTORY(Templates dbecl_vis_sources)

ADD_LIBRARY(dbecl_vis ${dbecl_vis_sources})

TARGET_LINK_LIBRARIES(dbecl_vis dbecl_pro bvis1)


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)


ENDIF (VXL_VGUI_FOUND)
