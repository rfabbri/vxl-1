# This is brcv/shp/dbskfg/vis/CMakeLists.txt
# Brown 2D Composite Graph Computation Library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

IF (VXL_VGUI_FOUND)

SET(dbskfg_vis_sources

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

ADD_LIBRARY(dbskfg_vis ${dbskfg_vis_sources})

TARGET_LINK_LIBRARIES(dbskfg_vis dbskfg_pro bvis1)

ENDIF (VXL_VGUI_FOUND)
