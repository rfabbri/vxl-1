# Detection for filters
# blem/contrib/biotree/resc/imgr/CMakeLists.txt

include_directories(
${VXL_BRL_INCLUDE_DIR}/bbas ${VXL_GEL_INCLUDE_DIR})

# for scanco visualization
set(imgr_ff_vis_sources vis_manager.cxx  vis_manager.h)
add_library(imgr_ff_vis ${imgr_ff_vis_sources})
target_link_libraries( imgr_ff_vis imgr_file_format bgui vgui vil vbl )

# for skyscan visualization
set(imgr_skyscan_vis_sources skyscan_vis_manager.cxx  skyscan_vis_manager.h)
add_library(imgr_skyscan_vis ${imgr_skyscan_vis_sources})
target_link_libraries( imgr_skyscan_vis imgr_file_format bgui vgui vil vbl vsol)

# The executable tool
add_subdirectory(bin)
