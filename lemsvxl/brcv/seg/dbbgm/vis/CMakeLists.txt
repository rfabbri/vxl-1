# Visualization for Brown background modeling library
# brcv/seg/dbbgm/vis/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/breye1 )

if (VXL_VGUI_FOUND)

set(dbbgm_vis_sources
   
   dbbgm_distribution_image_tableau.h       dbbgm_distribution_image_tableau.cxx
   dbbgm_distribution_image_tableau_sptr.h
   dbbgm_distribution_image_displayer.h     dbbgm_distribution_image_displayer.cxx
   dbbgm_inspector_tool.h                   dbbgm_inspector_tool.cxx

   dbbgm_image_tableau.h                    dbbgm_image_tableau.cxx
   dbbgm_image_tableau_sptr.h
   dbbgm_image_displayer.h                  dbbgm_image_displayer.cxx
)

add_library(dbbgm_vis  ${dbbgm_vis_sources})

target_link_libraries(dbbgm_vis dbbgm_pro bvis1 bgui vgui)

endif (VXL_VGUI_FOUND)
