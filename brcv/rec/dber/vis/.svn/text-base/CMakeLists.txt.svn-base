# This is brcv/rec/dber/vis/CMakeLists.txt
# Brown edge-based recognition library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/Gregistration )

IF (VXL_VGUI_FOUND)

SET(dber_vis_sources

#views

#tableaux
  dber_edge_match_tableau.h           dber_edge_match_tableau.cxx
  dber_edge_match_tableau_sptr.h
  
#displayers  
  dber_edge_match_displayer.h         dber_edge_match_displayer.cxx
  dber_instance_displayer.h           dber_instance_displayer.cxx
#tools
  dber_instance_tools.h              dber_instance_tools.cxx
)

ADD_LIBRARY(dber_vis ${dber_vis_sources})

TARGET_LINK_LIBRARIES(dber_vis dber_pro bvis1)

ENDIF (VXL_VGUI_FOUND)
