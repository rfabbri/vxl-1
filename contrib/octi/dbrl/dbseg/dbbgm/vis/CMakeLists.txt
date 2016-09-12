# Visualization for Brown background modeling library
# brcv/seg/dbbgm/vis/CMakeLists.txt

include_directories(${brl_SOURCE_DIR}/bbas)
include( ${MODULE_PATH}/UseVGUI.cmake )
if (HAS_VGUI)
set(dbbgmo_vis_sources
    dbbgm_inspector_tool.h                   dbbgm_inspector_tool.cxx
   dbbgm_image_tableau.h                     dbbgm_image_tableau.cxx
   dbbgm_image_tableau_sptr.h
                   
)

add_library(dbbgmo_vis  ${dbbgmo_vis_sources})

target_link_libraries(dbbgmo_vis dbbgmo_pro bgui)

endif (HAS_VGUI)
