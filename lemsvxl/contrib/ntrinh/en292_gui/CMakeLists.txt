# contrib/ntrinh/en292_gui/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

include_directories( ${lems_SOURCE_DIR}/contrib/ntrinh )

set(en292_gui_sources
   en292_gui_menu.cxx       en292_gui_menu.h
   en292_gui_main.cxx
)

add_executable(en292_gui ${en292_gui_sources})

target_link_libraries( en292_gui bvis1 en292_pro )


endif (VXL_VGUI_FOUND)
