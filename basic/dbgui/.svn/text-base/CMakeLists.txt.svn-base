# This is basic/dbgui/CMakeLists.txt
# Development Brown GUI Library

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )

IF (VXL_VGUI_FOUND)

SET(dbgui_sources
   dbgui_utils.cxx                 dbgui_utils.h
   dbgui_soview2D.cxx              dbgui_soview2D.h

   dbgui_gnd_cal_tableau.cxx       dbgui_gnd_cal_tableau.h      dbgui_gnd_cal_tableau_sptr.h
)

AUX_SOURCE_DIRECTORY(Templates dbgui_sources)

ADD_LIBRARY(dbgui  ${dbgui_sources})

TARGET_LINK_LIBRARIES(dbgui vgui gl2ps dbgl_algo vpgl_algo )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

ENDIF (VXL_VGUI_FOUND)
