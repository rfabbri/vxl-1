# Detection for filters
# blem/contrib/biotree/resc/imgr/CMakeLists.txt

INCLUDE_DIRECTORIES(
${VXL_BRL_INCLUDE_DIR}/bbas ${VXL_GEL_INCLUDE_DIR})

# for scanco visualization
SET(imgr_ff_vis_sources vis_manager.cxx  vis_manager.h)
ADD_LIBRARY(imgr_ff_vis ${imgr_ff_vis_sources})
TARGET_LINK_LIBRARIES( imgr_ff_vis imgr_file_format bgui vgui vil vbl )

# for skyscan visualization
SET(imgr_skyscan_vis_sources skyscan_vis_manager.cxx  skyscan_vis_manager.h)
ADD_LIBRARY(imgr_skyscan_vis ${imgr_skyscan_vis_sources})
TARGET_LINK_LIBRARIES( imgr_skyscan_vis imgr_file_format bgui vgui vil vbl vsol)

# The executable tool
SUBDIRS(bin)
