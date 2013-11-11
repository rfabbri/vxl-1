# contrib/ntrinh/en292_gui/CMakeLists.txt


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ntrinh )

SET(en292_gui_sources
   en292_gui_menu.cxx       en292_gui_menu.h
   en292_gui_main.cxx
)

ADD_EXECUTABLE(en292_gui ${en292_gui_sources})

TARGET_LINK_LIBRARIES( en292_gui bvis1 en292_pro )


ENDIF (VXL_VGUI_FOUND)
