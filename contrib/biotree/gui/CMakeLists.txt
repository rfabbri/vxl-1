# contrib/biotree/gui/CMakeLists.txt


IF (VXL_VGUI_FOUND)
IF (BUILD_BREYE)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

SET(biotree_gui_sources
   biotree_menu.cxx       biotree_menu.h
   biotree_main.cxx
)


AUX_SOURCE_DIRECTORY(Templates bvis1_sources)

ADD_EXECUTABLE(biotree_gui ${biotree_gui_sources})

TARGET_LINK_LIBRARIES(biotree_gui bvis1 biolung_process biocts_process)
ENDIF (BUILD_BREYE)
ENDIF (VXL_VGUI_FOUND)
