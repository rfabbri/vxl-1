# This is brcv/rec/dbru/vis/CMakeLists.txt
# Brown Recognition Tools to aid Testing

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${VXL_OXL_INCLUDE_DIR} )

if (VXL_VGUI_FOUND)

set(dbru_vis_sources  

#tableaux
dbru_rcor_tableau.h                    dbru_rcor_tableau.cxx
dbru_rcor_tableau_sptr.h

#displayers
dbru_rcor_displayer.h                  dbru_rcor_displayer.cxx
dbru_osl_displayer.h                   dbru_osl_displayer.cxx

#tools
dbru_labeling_tool.h                   dbru_labeling_tool.cxx
dbru_matching_tool.h                   dbru_matching_tool.cxx
dbru_rcor_tool.h                       dbru_rcor_tool.cxx
dbru_osl_tools.h                       dbru_osl_tools.cxx


)

add_library(dbru_vis ${dbru_vis_sources})

target_link_libraries(dbru_vis bvis1 dbru dbdet)

endif (VXL_VGUI_FOUND)
