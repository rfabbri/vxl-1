# contrib/ntrinh/dbsksp_gui/CMakeLists.txt


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/ntrinh )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )


SET(dbsksp_gui_sources
   dbsksp_gui_menu.cxx       dbsksp_gui_menu.h
   dbsksp_gui_main.cxx
)

ADD_EXECUTABLE(dbsksp_gui ${dbsksp_gui_sources})

TARGET_LINK_LIBRARIES( dbsksp_gui bvis1 dbsksp_vis dbsksp_pro dbsol_vis dbsol_pro dbsk2d_pro dbsk2d_vis dbskr_pro dbskr_vis dbcvr_pro dbcvr_vis dbdet_pro dbdet_vis dbsks_pro dbsks_vis dbgui)


# Small GUI
SET(dbsksp_small_gui_sources
   dbsksp_small_gui_menu.cxx       dbsksp_small_gui_menu.h
   dbsksp_small_gui_main.cxx
)
ADD_EXECUTABLE(dbsksp_small_gui ${dbsksp_small_gui_sources})
TARGET_LINK_LIBRARIES( dbsksp_small_gui bvis1 dbsksp_pro dbsksp_vis dbsol_vis dbsol_pro dbdet_pro dbsks_vis dbsk2d_pro dbsk2d_vis dbgui)


ENDIF (VXL_VGUI_FOUND)
