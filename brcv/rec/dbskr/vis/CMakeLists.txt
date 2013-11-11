# This is brcv/rec/dbskr/vis/CMakeLists.txt
# Brown shock recognition library

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

IF (VXL_VGUI_FOUND)

SET(dbskr_vis_sources

#views

#tableaux
  dbskr_shock_match_tableau.h           dbskr_shock_match_tableau.cxx
  dbskr_shock_match_tableau_sptr.h

  dbskr_shock_patch_tableau.h           dbskr_shock_patch_tableau.cxx
  dbskr_shock_patch_tableau_sptr.h

  dbskr_shock_patch_match_tableau.h     dbskr_shock_patch_match_tableau.cxx
  dbskr_shock_patch_match_tableau_sptr.h
  
#displayers  
  dbskr_shock_match_displayer.h         dbskr_shock_match_displayer.cxx
  dbskr_shock_patch_displayer.h         dbskr_shock_patch_displayer.cxx
  dbskr_shock_patch_match_displayer.h   dbskr_shock_patch_match_displayer.cxx
  
#tools
  dbskr_debug_scurve_tool.h             dbskr_debug_scurve_tool.cxx
  dbskr_subgraph_scurve_tool.h          dbskr_subgraph_scurve_tool.cxx
#  dbskr_shock_patch_view_tool.h         dbskr_shock_patch_view_tool.cxx
  dbskr_visualize_path_scurve_tool.h     dbskr_visualize_path_scurve_tool.cxx

)

ADD_LIBRARY(dbskr_vis ${dbskr_vis_sources})

TARGET_LINK_LIBRARIES(dbskr_vis dbskr_algo dbskr_pro bvis1 ) 
# dbsk2d_vis)

ENDIF (VXL_VGUI_FOUND)
