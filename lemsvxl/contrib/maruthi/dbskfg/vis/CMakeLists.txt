# This is brcv/shp/dbskfg/vis/CMakeLists.txt
# Brown 2D Composite Graph Computation Library

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )

if (VXL_VGUI_FOUND)

set(dbskfg_vis_sources

  #views
  dbskfg_soview_composite_node.h dbskfg_soview_composite_node.cxx
  dbskfg_soview_composite_link.h dbskfg_soview_composite_link.cxx
  dbskfg_soview_rag_node.h dbskfg_soview_rag_node.cxx
  dbskfg_soview_transform.h dbskfg_soview_transform.cxx

  #tableaux
  dbskfg_composite_graph_tableau.h      dbskfg_composite_graph_tableau.cxx
  dbskfg_composite_graph_tableau_sptr.h

  #displayers  
  dbskfg_composite_graph_displayer.h    dbskfg_composite_graph_displayer.cxx

  #tools
  dbskfg_composite_graph_transform_tool.h 
  dbskfg_composite_graph_transform_tool.cxx
)

add_library(dbskfg_vis ${dbskfg_vis_sources})

target_link_libraries(dbskfg_vis dbskfg_pro bvis1)

endif (VXL_VGUI_FOUND)
