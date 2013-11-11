# contrib/ntrinh/dbsksp_gui/CMakeLists.txt


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ntrinh )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )



# Skeleton search gui
SET(skeleton_search_menu_sources
    skeleton_search_menu.cxx       skeleton_search_menu.h
    skeleton_search_main.cxx
)
ADD_EXECUTABLE(skeleton_search ${skeleton_search_menu_sources})
TARGET_LINK_LIBRARIES(skeleton_search bvis1 dbsksp_pro dbsksp_vis dbsks_pro dbsks_vis dbdet_algo dbdet_vis dbdet_pro dbgui)


ENDIF (VXL_VGUI_FOUND)
