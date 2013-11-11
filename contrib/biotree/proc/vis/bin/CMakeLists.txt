# biotree/resc/imgr/imgrv/bin/CMakeLists.txt

SET(bioproc_view3d_sources view3d.cxx view3d_menus.cxx view3d_menus.h)

ADD_EXECUTABLE (bioproc_view3d ${bioproc_view3d_sources})

TARGET_LINK_LIBRARIES(bioproc_view3d bioproc_vis vgui)
