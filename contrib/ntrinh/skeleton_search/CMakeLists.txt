# contrib/ntrinh/dbsksp_gui/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

include_directories( ${lems_SOURCE_DIR}/contrib/ntrinh )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/basic )



# Skeleton search gui
set(skeleton_search_menu_sources
    skeleton_search_menu.cxx       skeleton_search_menu.h
    skeleton_search_main.cxx
)
add_executable(skeleton_search ${skeleton_search_menu_sources})
target_link_libraries(skeleton_search bvis1 dbsksp_pro dbsksp_vis dbsks_pro dbsks_vis dbdet_algo dbdet_vis dbdet_pro dbgui)


endif (VXL_VGUI_FOUND)
