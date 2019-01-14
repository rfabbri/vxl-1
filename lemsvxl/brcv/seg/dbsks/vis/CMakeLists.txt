# seg/dbsks/vis/CMakeLists.txt
# Brown Visualization Library for Model-based segmentation using shock

include_directories( ${VXL_BRL_INCLUDE_DIR})
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )


if (VXL_VGUI_FOUND)

set( dbsks_vis_sources
  dbsks_examine_graph_cost_tool.h   dbsks_examine_graph_cost_tool.cxx
  dbsks_examine_xgraph_cost_tool.h  dbsks_examine_xgraph_cost_tool.cxx
  dbsks_xshock_cost_tool.h         dbsks_xshock_cost_tool.cxx
)

add_library(dbsks_vis ${dbsks_vis_sources})

target_link_libraries(dbsks_vis dbsks_algo dbdet_vis bvis1)

endif (VXL_VGUI_FOUND)

