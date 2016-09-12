


include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas ${VXL_GEL_INCLUDE_DIR} ${VXL_GEL_INCLUDE_DIR}/mrc ${biotree_SOURCE_DIR})
#message(${biotree_SOURCE_DIR})

# for skyscan roi_pickualization
set(skyscan_roi_pick_sources skyscan_roi_pick_manager.cxx  skyscan_roi_pick_manager.h)
add_library(skyscan_roi_pick ${skyscan_roi_pick_sources})
target_link_libraries(skyscan_roi_pick imgr_file_format bgui vgui vil vbl vsol)

# The executable tool
add_subdirectory(bin)
