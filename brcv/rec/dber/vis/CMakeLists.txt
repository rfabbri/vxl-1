# This is brcv/rec/dber/vis/CMakeLists.txt
# Brown edge-based recognition library

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${lems_SOURCE_DIR}/contrib/Gregistration )

if (VXL_VGUI_FOUND)

set(dber_vis_sources

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

add_library(dber_vis ${dber_vis_sources})

target_link_libraries(dber_vis dber_pro bvis1)

endif (VXL_VGUI_FOUND)
