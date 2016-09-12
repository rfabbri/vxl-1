# contrib/ntrinh/dbsksp_gui/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

include_directories( ${lems_SOURCE_DIR}/contrib/ntrinh )
include_directories( ${lems_SOURCE_DIR}/brcv/shp )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/basic )


set(dbsksp_gui_sources
   dbsksp_gui_menu.cxx       dbsksp_gui_menu.h
   dbsksp_gui_main.cxx
)

add_executable(dbsksp_gui ${dbsksp_gui_sources})

target_link_libraries( dbsksp_gui bvis1 dbsksp_vis dbsksp_pro dbsol_vis dbsol_pro dbsk2d_pro dbsk2d_vis dbskr_pro dbskr_vis dbcvr_pro dbcvr_vis dbdet_pro dbdet_vis dbsks_pro dbsks_vis dbgui)


# Small GUI
set(dbsksp_small_gui_sources
   dbsksp_small_gui_menu.cxx       dbsksp_small_gui_menu.h
   dbsksp_small_gui_main.cxx
)
add_executable(dbsksp_small_gui ${dbsksp_small_gui_sources})
target_link_libraries( dbsksp_small_gui bvis1 dbsksp_pro dbsksp_vis dbsol_vis dbsol_pro dbdet_pro dbsks_vis dbsk2d_pro dbsk2d_vis dbgui)


endif (VXL_VGUI_FOUND)
