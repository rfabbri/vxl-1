# contrib/edge_det/gui/CMakeLists.txt

IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/rec )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/shp )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/trk )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/Gregistration )

SET(sel_gui_sources
   sel_gui_menu.cxx       sel_gui_menu.h
   sel_gui_main.cxx
)
ADD_EXECUTABLE(sel_gui ${sel_gui_sources})
TARGET_LINK_LIBRARIES(sel_gui vidpro1 vidl1 bvis1 dbgl_algo dbsol_pro dbil_pro dbdet_pro dbdet_vis dbgui edge_det_pro edge_det_vis)

ENDIF (VXL_VGUI_FOUND)
