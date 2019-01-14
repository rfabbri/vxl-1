# contrib/han/gui/CMakeLists.txt



include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${LEMS_SOURCE_DIR}/breye )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/contrib/han )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )

set(han_gui_sources
   han_menu.cxx       han_menu.h
   han_main.cxx
)


aux_source_directory(Templates bvis_sources)

add_executable(han_gui ${han_gui_sources})


target_link_libraries(han_gui dbshadow_pro dbcri_pro dbcri_accu_pro dbcri_accu_edge_pro dbcri_whole_pro dbcri_whole_flip_pro bvis dbgui vidpro vidl1 dbecl brct vgl vgl_algo)



#endif (VXL_VGUI_FOUND)
