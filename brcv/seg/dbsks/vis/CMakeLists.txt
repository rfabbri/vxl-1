# seg/dbsks/vis/CMakeLists.txt
# Brown Visualization Library for Model-based segmentation using shock

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )


IF (VXL_VGUI_FOUND)

SET( dbsks_vis_sources
  dbsks_examine_graph_cost_tool.h   dbsks_examine_graph_cost_tool.cxx
  dbsks_examine_xgraph_cost_tool.h  dbsks_examine_xgraph_cost_tool.cxx
  dbsks_xshock_cost_tool.h         dbsks_xshock_cost_tool.cxx
)

ADD_LIBRARY(dbsks_vis ${dbsks_vis_sources})

TARGET_LINK_LIBRARIES(dbsks_vis dbsks_algo dbdet_vis bvis1)

ENDIF (VXL_VGUI_FOUND)

