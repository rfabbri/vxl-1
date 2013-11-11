# This is brcv/rec/dbcvr/vis/CMakeLists.txt
# Brown Curve Matching Library Visualization

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

IF (VXL_VGUI_FOUND)

SET(dbcvr_vis_sources

#tableaux
  dbcvr_curvematch_tableau.h          dbcvr_curvematch_tableau.cxx
  dbcvr_curvematch_tableau_sptr.h

#displayers  
  dbcvr_curvematch_displayer.h        dbcvr_curvematch_displayer.cxx
  
)

ADD_LIBRARY(dbcvr_vis ${dbcvr_vis_sources})

TARGET_LINK_LIBRARIES(dbcvr_vis dbcvr_pro bvis1)

ENDIF (VXL_VGUI_FOUND)
