# contrib/starter_kit/gui/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

set(starter_kit_gui_sources
   starter_kit_menu.cxx       starter_kit_menu.h
   starter_kit_main.cxx
)


aux_source_directory(Templates bvis_sources)

add_executable(starter_kit_gui ${starter_kit_gui_sources})

target_link_libraries(starter_kit_gui bvis dbgui vidpro vidl1)


endif (VXL_VGUI_FOUND)
