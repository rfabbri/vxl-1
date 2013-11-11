# Visualization for Brown background modeling library
# brcv/seg/dbbgm/vis/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

IF (VXL_VGUI_FOUND)

SET(dbbgm_vis_sources
   
   dbbgm_distribution_image_tableau.h       dbbgm_distribution_image_tableau.cxx
   dbbgm_distribution_image_tableau_sptr.h
   dbbgm_distribution_image_displayer.h     dbbgm_distribution_image_displayer.cxx
   dbbgm_inspector_tool.h                   dbbgm_inspector_tool.cxx

   dbbgm_image_tableau.h                    dbbgm_image_tableau.cxx
   dbbgm_image_tableau_sptr.h
   dbbgm_image_displayer.h                  dbbgm_image_displayer.cxx
)

ADD_LIBRARY(dbbgm_vis  ${dbbgm_vis_sources})

TARGET_LINK_LIBRARIES(dbbgm_vis dbbgm_pro bvis1)

ENDIF (VXL_VGUI_FOUND)
