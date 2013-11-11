# Visualization for Brown segmentation library
# brcv/seg/dbdet/vis/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )

SET(dbdet_vis_sources
   
   dbdet_livewire_tool.cxx                dbdet_livewire_tool.h 
   dbdet_multiview_livewire_tool.cxx      dbdet_multiview_livewire_tool.h
   dbdet_medial_livewire_tool.cxx         dbdet_medial_livewire_tool.h

   dbdet_edgemap_displayer.cxx            dbdet_edgemap_displayer.h
   dbdet_edgemap_tableau.cxx              dbdet_edgemap_tableau.h
   dbdet_edgemap_tableau_sptr.h
   
   dbdet_crop_edgemap_tool.cxx            dbdet_crop_edgemap_tool.h
   dbdet_query_linking_tool.cxx           dbdet_query_linking_tool.h
   
   dbdet_keypoint_displayer.cxx           dbdet_keypoint_displayer.h
   dbdet_keypoint_soview2D.cxx            dbdet_keypoint_soview2D.h
   
   dbdet_sel_displayer.cxx                dbdet_sel_displayer.h
   dbdet_sel_tableau_commands.h 
   dbdet_sel_tableau.cxx                  dbdet_sel_tableau.h       
   dbdet_sel_tableau_sptr.h
   
   dbdet_sel_edge_linking_tool.cxx        dbdet_sel_edge_linking_tool.h
   dbdet_sel_contour_explorer_tool.cxx    dbdet_sel_contour_explorer_tool.h
   dbdet_sel_explorer_tool.cxx            dbdet_sel_explorer_tool.h
   dbdet_sel_curvelet_linking_tool.cxx    dbdet_sel_curvelet_linking_tool.h
   dbdet_sel_DHT_tool.cxx                 dbdet_sel_DHT_tool.h
   dbdet_sel_CC_linking_tool.cxx          dbdet_sel_CC_linking_tool.h
   
   dbdet_crop_curvelet_map_tool.cxx       dbdet_crop_curvelet_map_tool.h
)

ADD_LIBRARY(dbdet_vis ${dbdet_vis_sources})

TARGET_LINK_LIBRARIES(dbdet_vis dbdet_pro dbgl_algo brip)

