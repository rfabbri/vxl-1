# Detection for filters
# blem/contrib/biotree/resc/imgr/CMakeLists.txt

if(VXL_VGUI_FOUND)
  include( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
endif (VXL_VGUI_FOUND)

include_directories( ${VXL_GEL_INCLUDE_DIR})
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories(${VXL_BRL_INCLUDE_DIR}/bbas)
include_directories( ${lems_SOURCE_DIR}/db3p )

set(bioproc_vis_sources
vis_manager.cxx  vis_manager.h
)


add_library(bioproc_vis ${bioproc_vis_sources})


target_link_libraries( bioproc_vis imgr_file_format bgui vgui vil vbl biov )

add_executable(bioproc_response_vis bioproc_response_vis.cxx)
target_link_libraries(bioproc_response_vis bioproc bioproc_io bgui3d biov)

add_executable(bioproc_splr_response_vis bioproc_splr_response_vis.cxx)
target_link_libraries(bioproc_splr_response_vis bioproc bioproc_io bgui3d biov geom)

add_executable(bioproc_response_vis_dialog_box bioproc_response_vis_dialog_box.cxx)
target_link_libraries(bioproc_response_vis_dialog_box bioproc bgui3d biov)

add_executable(bioproc_edge_response_vis bioproc_edge_response_vis.cxx)
target_link_libraries(bioproc_edge_response_vis bioproc bioproc_io bgui3d biov)

set(bio_vis_sources
bio_vis.cxx  bio_vis.h
)

add_executable(bio_vis ${bio_vis_sources})

target_link_libraries(bio_vis bioproc bioproc_io bgui3d biov vil vil3d geom)

# The executable tool
add_subdirectory(bin)

