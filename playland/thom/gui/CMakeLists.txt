# playland/thom/gui/CMakeLists.txt



IF (VXL_VGUI_FOUND)


INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

SET(thom_gui_sources

   thom_menu.cxx       thom_menu.h
   thom_main.cxx
)

AUX_SOURCE_DIRECTORY(Templates bvis_sources)

ADD_EXECUTABLE(thom_gui ${thom_gui_sources})

TARGET_LINK_LIBRARIES(thom_gui bvis dbgui vidpro vidl)

ENDIF (VXL_VGUI_FOUND)
