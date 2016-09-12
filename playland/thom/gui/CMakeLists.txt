# playland/thom/gui/CMakeLists.txt



if (VXL_VGUI_FOUND)


include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )

set(thom_gui_sources

   thom_menu.cxx       thom_menu.h
   thom_main.cxx
)

aux_source_directory(Templates bvis_sources)

add_executable(thom_gui ${thom_gui_sources})

target_link_libraries(thom_gui bvis dbgui vidpro vidl)

endif (VXL_VGUI_FOUND)
