# This is brcv/rec/dbru/vis/CMakeLists.txt
# Brown Recognition Tools to aid Testing

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )

IF (VXL_VGUI_FOUND)

SET(face_vis_sources  

#displayers
dbru_facedb_displayer.h                   dbru_facedb_displayer.cxx

#tools
dbru_facedb_tools.h                       dbru_facedb_tools.cxx

)

ADD_LIBRARY(face_vis ${face_vis_sources})

TARGET_LINK_LIBRARIES(face_vis bvis1 dbru dbdet)

ENDIF (VXL_VGUI_FOUND)
