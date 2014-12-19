# contrib/firat/dbsksp_gui/CMakeLists.txt
IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/firat )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

# Small GUI
SET(dbsksp_gui_firat_sources
   dbsksp_small_gui_menu.cxx       dbsksp_small_gui_menu.h
   dbsksp_small_gui_main.cxx
)
ADD_EXECUTABLE(dbsksp_gui_firat ${dbsksp_gui_firat_sources})
TARGET_LINK_LIBRARIES( dbsksp_gui_firat vidl1 bvis1 dbsksp_pro dbsksp_vis dbsol_vis dbsol_pro dbdet_pro dbsks_vis dbsk2d_pro dbsks_pro dbsk2d_vis dbgui dbdet_vis)


ENDIF (VXL_VGUI_FOUND)
