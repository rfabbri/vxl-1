# biotree/resc/imgr/imgrv/bin/CMakeLists.txt

include_directories( ${VXLCORE_INCLUDE_DIR}
${biotree_SOURCE_DIR}/resc/imgr/file_formats )

# for scanco visualization
set(view3d_sources view3d.cxx view3d_menus.cxx view3d_menus.h)
add_executable (imgr_ff_view3d ${view3d_sources})
target_link_libraries(imgr_ff_view3d imgr_ff_vis vgui)

# for skyscan visualization
set(skyscan_view3d_sources skyscan_view3d.cxx skyscan_view3d_menus.cxx skyscan_view3d_menus.h)
add_executable (imgr_skyscan_view3d ${skyscan_view3d_sources})
target_link_libraries(imgr_skyscan_view3d imgr_skyscan_vis vgui)
