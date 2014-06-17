# contrib/mauro/gui/CMakeLists.txt


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

SET(mauro_gui_sources
   mauro_menu.cxx       mauro_menu.h
   mauro_main.cxx
)


AUX_SOURCE_DIRECTORY(Templates bvis_sources)

ADD_EXECUTABLE(mauro_gui ${mauro_gui_sources})

TARGET_LINK_LIBRARIES(mauro_gui bvis dbgui vidpro vidl1)


ENDIF (VXL_VGUI_FOUND)
