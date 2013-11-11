


INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas ${VXL_GEL_INCLUDE_DIR} ${VXL_GEL_INCLUDE_DIR}/mrc ${biotree_SOURCE_DIR})
#MESSAGE(${biotree_SOURCE_DIR})

# for skyscan roi_pickualization
SET(skyscan_roi_pick_sources skyscan_roi_pick_manager.cxx  skyscan_roi_pick_manager.h)
ADD_LIBRARY(skyscan_roi_pick ${skyscan_roi_pick_sources})
TARGET_LINK_LIBRARIES(skyscan_roi_pick imgr_file_format bgui vgui vil vbl vsol)

# The executable tool
SUBDIRS(bin)
