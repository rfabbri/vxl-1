# Visualization for Brown background modeling library
# brcv/seg/dbbgm/vis/CMakeLists.txt

INCLUDE_DIRECTORIES(${brl_SOURCE_DIR}/bbas)
INCLUDE( ${MODULE_PATH}/UseVGUI.cmake )
IF (HAS_VGUI)
SET(dbbgmo_vis_sources
    dbbgm_inspector_tool.h                   dbbgm_inspector_tool.cxx
   dbbgm_image_tableau.h                     dbbgm_image_tableau.cxx
   dbbgm_image_tableau_sptr.h
                   
)

ADD_LIBRARY(dbbgmo_vis  ${dbbgmo_vis_sources})

TARGET_LINK_LIBRARIES(dbbgmo_vis dbbgmo_pro bgui)

ENDIF (HAS_VGUI)
