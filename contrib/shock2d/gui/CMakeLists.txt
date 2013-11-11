# contrib/shock2d/gui/CMakeLists.txt

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
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/maruthi )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/maruthi/vlfeat-0.9.16 )

LINK_DIRECTORIES(${lems_SOURCE_DIR}/contrib/maruthi/vlfeat-0.9.16/bin/glnxa64 )

#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/Gregistration )

SET(shock2d_gui_sources
   shock2d_gui_menu.cxx       shock2d_gui_menu.h
   shock2d_gui_main.cxx
)
ADD_EXECUTABLE(shock2d_gui ${shock2d_gui_sources})
TARGET_LINK_LIBRARIES(shock2d_gui vidpro1 bvis1 dbsk2d_pro dbsk2d_vis dbskr_pro dber_pro dber_vis dbskr_vis dbdet_pro dbdet_vis dbinfo_pro dbsol_pro dbru_algo dbil_pro dbru_pro dbru_vis dbskfg dbskfg_vis dbskfg_pro con_graph_pro )

ENDIF (VXL_VGUI_FOUND)
