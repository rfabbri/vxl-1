# This is brcv/mvg/dbvrl/vis/CMakeLists.txt
# Brown Video Registration Visualization library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

IF (VXL_VGUI_FOUND)

SET(dbvrl_vis_sources
   dbvrl_region_tableau.cxx      dbvrl_region_tableau.h       dbvrl_region_tableau_sptr.h
   dbvrl_region_displayer.cxx    dbvrl_region_displayer.h
   dbvrl_region_tools.cxx        dbvrl_region_tools.h
)

#AUX_SOURCE_DIRECTORY(Templates dbvrl_vis_sources)

ADD_LIBRARY(dbvrl_vis ${dbvrl_vis_sources})

TARGET_LINK_LIBRARIES(dbvrl_vis dbvrl_pro bvis1)


IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)


ENDIF (VXL_VGUI_FOUND)
