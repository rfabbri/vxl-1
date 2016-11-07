# contrib/firat/dbsksp_gui/CMakeLists.txt
if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

include_directories( ${lems_SOURCE_DIR}/contrib/firat )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/basic )

# Small GUI
set(dbsksp_gui_firat_sources
   dbsksp_small_gui_menu.cxx       dbsksp_small_gui_menu.h
   dbsksp_small_gui_main.cxx
)
add_executable(dbsksp_gui_firat ${dbsksp_gui_firat_sources})
target_link_libraries( dbsksp_gui_firat bvis1 dbsksp_pro dbsksp_vis dbsol_vis dbsol_pro dbdet_pro dbsks_vis dbsk2d_pro dbsks_pro dbsk2d_vis dbgui dbdet_vis)


endif (VXL_VGUI_FOUND)
