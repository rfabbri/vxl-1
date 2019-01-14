# This is brcv/rec/dbskr/vis/CMakeLists.txt
# Brown shock recognition library

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )

if (VXL_VGUI_FOUND)
find_package(OpenGL)

set(dbskr_vis_sources

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

add_library(dbskr_vis ${dbskr_vis_sources})

target_link_libraries(dbskr_vis dbskr_algo dbskr_pro dbsk2d_vis bvis1 vgui  ${OPENGL_LIBRARIES})
# dbsk2d_vis)

endif (VXL_VGUI_FOUND)
