# Detection for filters
# blem/contrib/biotree/resc/imgr/CMakeLists.txt

IF(VXL_VGUI_FOUND)
  INCLUDE( ${VXL_CMAKE_DIR}/UseBGUI3D.cmake)
ENDIF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )

SET(bioproc_vis_sources
vis_manager.cxx  vis_manager.h
)


ADD_LIBRARY(bioproc_vis ${bioproc_vis_sources})


TARGET_LINK_LIBRARIES( bioproc_vis imgr_file_format bgui vgui vil vbl biov )

ADD_EXECUTABLE(bioproc_response_vis bioproc_response_vis.cxx)
TARGET_LINK_LIBRARIES(bioproc_response_vis bioproc bioproc_io bgui3d biov)

ADD_EXECUTABLE(bioproc_splr_response_vis bioproc_splr_response_vis.cxx)
TARGET_LINK_LIBRARIES(bioproc_splr_response_vis bioproc bioproc_io bgui3d biov geom)

ADD_EXECUTABLE(bioproc_response_vis_dialog_box bioproc_response_vis_dialog_box.cxx)
TARGET_LINK_LIBRARIES(bioproc_response_vis_dialog_box bioproc bgui3d biov)

ADD_EXECUTABLE(bioproc_edge_response_vis bioproc_edge_response_vis.cxx)
TARGET_LINK_LIBRARIES(bioproc_edge_response_vis bioproc bioproc_io bgui3d biov)

SET(bio_vis_sources
bio_vis.cxx  bio_vis.h
)

ADD_EXECUTABLE(bio_vis ${bio_vis_sources})

TARGET_LINK_LIBRARIES(bio_vis bioproc bioproc_io bgui3d biov vil vil3d geom)

# The executable tool
SUBDIRS(bin)

