# breyes/bvis/examples/CMakeLists.txt


if (VXL_VGUI_FOUND)

set(brown_eyes_sources

   bvis_brown_eyes_menu.cxx       bvis_brown_eyes_menu.h
   bvis_brown_eyes_main.cxx
)

aux_source_directory(Templates bvis_sources)

add_executable(brown_eyes ${brown_eyes_sources})

target_link_libraries(brown_eyes bvis bpro vbl_io vbl)

endif (VXL_VGUI_FOUND)
