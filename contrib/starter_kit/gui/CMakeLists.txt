# contrib/starter_kit/gui/CMakeLists.txt


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

SET(starter_kit_gui_sources
   starter_kit_menu.cxx       starter_kit_menu.h
   starter_kit_main.cxx
)


AUX_SOURCE_DIRECTORY(Templates bvis_sources)

ADD_EXECUTABLE(starter_kit_gui ${starter_kit_gui_sources})

TARGET_LINK_LIBRARIES(starter_kit_gui bvis dbgui vidpro vidl1)


ENDIF (VXL_VGUI_FOUND)
