# contrib/ozge/gui/CMakeLists.txt

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
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1  )


SET(matching_gui_sources
   matching_gui_menu.cxx       matching_gui_menu.h
   matching_gui_main.cxx
)

AUX_SOURCE_DIRECTORY(Templates matching_gui_sources)

ADD_EXECUTABLE(matching_gui ${matching_gui_sources})

TARGET_LINK_LIBRARIES(matching_gui matching_pro dbdet_pro dbbgm_pro dbbgm_vis vidpro1 bvis1 dbsk2d_pro dbskr_pro dbskr_vis dbinfo_pro dbru_algo dbru_pro dbru_vis dbacm_pro dbsol_pro dbdet_pro dbsol_pro dbcvr_pro)

ENDIF (VXL_VGUI_FOUND)
