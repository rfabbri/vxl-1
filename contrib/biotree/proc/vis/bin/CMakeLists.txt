# biotree/resc/imgr/imgrv/bin/CMakeLists.txt

set(bioproc_view3d_sources view3d.cxx view3d_menus.cxx view3d_menus.h)

add_executable (bioproc_view3d ${bioproc_view3d_sources})

target_link_libraries(bioproc_view3d bioproc_vis vgui)
