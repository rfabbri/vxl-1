

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} ${biotree_SOURCE_DIR}/resc/imgr/file_formats )

LINK_DIRECTORIES(${VXL_LIBRARY_DIR} ${LEMS_LIBRARY_DIR})
# for skyscan roi_pickualization
SET(skyscan_roi_pick_sources skyscan_roi_pick.cxx skyscan_roi_pick_menus.cxx skyscan_roi_pick_menus.h)
ADD_EXECUTABLE (skyscan_roi ${skyscan_roi_pick_sources})
TARGET_LINK_LIBRARIES(skyscan_roi skyscan_roi_pick vgui)
