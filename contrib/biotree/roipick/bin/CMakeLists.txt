

include_directories( ${VXLCORE_INCLUDE_DIR} ${biotree_SOURCE_DIR}/resc/imgr/file_formats )

link_directories(${VXL_LIBRARY_DIR} ${LEMS_LIBRARY_DIR})
# for skyscan roi_pickualization
set(skyscan_roi_pick_sources skyscan_roi_pick.cxx skyscan_roi_pick_menus.cxx skyscan_roi_pick_menus.h)
add_executable (skyscan_roi ${skyscan_roi_pick_sources})
target_link_libraries(skyscan_roi skyscan_roi_pick vgui)
