# This is brcv/trk/dbinfo/vis/CMakeLists.txt
# Brown Tracking Visualization library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
#INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

IF (VXL_VGUI_FOUND)

SET(dbinfo_vis_sources
   dbinfo_track_displayer.cxx           dbinfo_track_displayer.h
   dbinfo_osl_displayer.cxx             dbinfo_osl_displayer.h
   dbinfo_track_tools.cxx               dbinfo_track_tools.h
   dbinfo_osl_tools.cxx                 dbinfo_osl_tools.h
)


ADD_LIBRARY(dbinfo_vis ${dbinfo_vis_sources})

TARGET_LINK_LIBRARIES(dbinfo_vis bvis1 dbinfo)


#IF(BUILD_TESTING)
#  SUBDIRS(tests)
#ENDIF(BUILD_TESTING)


ENDIF (VXL_VGUI_FOUND)
