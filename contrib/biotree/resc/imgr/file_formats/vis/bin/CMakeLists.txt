# biotree/resc/imgr/imgrv/bin/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR}
${biotree_SOURCE_DIR}/resc/imgr/file_formats )

# for scanco visualization
SET(view3d_sources view3d.cxx view3d_menus.cxx view3d_menus.h)
ADD_EXECUTABLE (imgr_ff_view3d ${view3d_sources})
TARGET_LINK_LIBRARIES(imgr_ff_view3d imgr_ff_vis vgui)

# for skyscan visualization
SET(skyscan_view3d_sources skyscan_view3d.cxx skyscan_view3d_menus.cxx skyscan_view3d_menus.h)
ADD_EXECUTABLE (imgr_skyscan_view3d ${skyscan_view3d_sources})
TARGET_LINK_LIBRARIES(imgr_skyscan_view3d imgr_skyscan_vis vgui)
