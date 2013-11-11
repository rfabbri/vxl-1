# This is brcv/rec/dbru/vis/CMakeLists.txt
# Brown Recognition Tools to aid Testing

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )

IF (VXL_VGUI_FOUND)

SET(dbru_vis_sources  

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

ADD_LIBRARY(dbru_vis ${dbru_vis_sources})

TARGET_LINK_LIBRARIES(dbru_vis bvis1 dbru dbdet)

ENDIF (VXL_VGUI_FOUND)
