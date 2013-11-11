# This is brcv/shp/dbsk2d/vis/CMakeLists.txt
# Brown 2D Shock Computation Library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

IF (VXL_VGUI_FOUND)

SET(dbsk2d_vis_sources

#views
  dbsk2d_bpoint_view.h             dbsk2d_bpoint_view.cxx
  dbsk2d_bline_view.h              dbsk2d_bline_view.cxx

#tableaux
  dbsk2d_shock_tableau.h           dbsk2d_shock_tableau.cxx
  dbsk2d_shock_tableau_sptr.h
  
  #dbsk2d_xshock_tableau.h         dbsk2d_xshock_tableau.cxx
  #dbsk2d_xshock_tableau_sptr.h

#displayers  
  dbsk2d_shock_displayer.h         dbsk2d_shock_displayer.cxx
  #dbsk2d_xshock_displayer.h       dbsk2d_xshock_displayer.cxx
  
#tools
  dbsk2d_ishock_tool.h                  dbsk2d_ishock_tool.cxx
  dbsk2d_ishock_highlight_tool.h        dbsk2d_ishock_highlight_tool.cxx
  dbsk2d_ishock_getinfo_tool.h          dbsk2d_ishock_getinfo_tool.cxx
  dbsk2d_ishock_detector_debug_tool.h   dbsk2d_ishock_detector_debug_tool.cxx
  dbsk2d_rich_map_query_tool.h          dbsk2d_rich_map_query_tool.cxx
  dbsk2d_shock_image_fragment_tool.h    dbsk2d_shock_image_fragment_tool.cxx
  
#shock transform tools
  dbsk2d_ishock_splice_transform_tool.h  dbsk2d_ishock_splice_transform_tool.cxx
  dbsk2d_ishock_gap_transform_tool.h     dbsk2d_ishock_gap_transform_tool.cxx
  dbsk2d_ishock_loop_transform_tool.h    dbsk2d_ishock_loop_transform_tool.cxx

  dbsk2d_shock_subgraph_tool.h           dbsk2d_shock_subgraph_tool.cxx
)

ADD_LIBRARY(dbsk2d_vis ${dbsk2d_vis_sources})

TARGET_LINK_LIBRARIES(dbsk2d_vis dbsk2d_pro dbsk2d_algo bvis1)

ENDIF (VXL_VGUI_FOUND)
