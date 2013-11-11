# contrib/han/gui/CMakeLists.txt



INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/han )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )

SET(han_gui_sources
   han_menu.cxx       han_menu.h
   han_main.cxx
)


AUX_SOURCE_DIRECTORY(Templates bvis_sources)

ADD_EXECUTABLE(han_gui ${han_gui_sources})


TARGET_LINK_LIBRARIES(han_gui dbshadow_pro dbcri_pro dbcri_accu_pro dbcri_accu_edge_pro dbcri_whole_pro dbcri_whole_flip_pro bvis dbgui vidpro vidl1 dbecl brct vgl vgl_algo)



#ENDIF (VXL_VGUI_FOUND)
