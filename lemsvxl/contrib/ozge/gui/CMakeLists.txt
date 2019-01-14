# contrib/ozge/gui/CMakeLists.txt

if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_MUL_INCLUDE_DIR} )
include_directories( ${VXL_OXL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/rec )
include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )
include_directories( ${LEMS_SOURCE_DIR}/contrib )


set(matching_gui_sources
   matching_gui_menu.cxx       matching_gui_menu.h
   matching_gui_main.cxx
)

aux_source_directory(Templates matching_gui_sources)

add_executable(matching_gui ${matching_gui_sources})

target_link_libraries(matching_gui matching_pro dbdet_pro dbbgm_pro dbbgm_vis vidpro1 bvis1 dbsk2d_pro dbskr_pro dbskr_vis dbinfo_pro dbru_algo dbru_pro dbru_vis dbacm_pro dbsol_pro dbdet_pro dbsol_pro)

endif (VXL_VGUI_FOUND)
