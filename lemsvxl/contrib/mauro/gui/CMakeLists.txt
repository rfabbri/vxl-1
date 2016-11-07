# contrib/mauro/gui/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

set(mauro_gui_sources
   mauro_menu.cxx       mauro_menu.h
   mauro_main.cxx
)


aux_source_directory(Templates bvis_sources)

add_executable(mauro_gui ${mauro_gui_sources})

target_link_libraries(mauro_gui bvis dbgui vidpro vidl1)


endif (VXL_VGUI_FOUND)
