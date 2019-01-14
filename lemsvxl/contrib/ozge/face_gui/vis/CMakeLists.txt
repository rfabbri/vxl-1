# This is brcv/rec/dbru/vis/CMakeLists.txt
# Brown Recognition Tools to aid Testing

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${VXL_OXL_INCLUDE_DIR} )

if (VXL_VGUI_FOUND)

set(face_vis_sources  

#displayers
dbru_facedb_displayer.h                   dbru_facedb_displayer.cxx

#tools
dbru_facedb_tools.h                       dbru_facedb_tools.cxx

)

add_library(face_vis ${face_vis_sources})

target_link_libraries(face_vis bvis1 dbru dbdet)

endif (VXL_VGUI_FOUND)
