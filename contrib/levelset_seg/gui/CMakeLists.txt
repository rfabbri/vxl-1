# contrib/levelset_seg/gui/CMakeLists.txt


IF (VXL_VGUI_FOUND)

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

# local repository paths
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )

SET(levelset_seg_gui_sources
   levelset_seg_menu.cxx          levelset_seg_menu.h
   levelset_seg_main.cxx
)


AUX_SOURCE_DIRECTORY(Templates bvis1_sources)

ADD_EXECUTABLE(levelset_seg_gui ${levelset_seg_gui_sources})

TARGET_LINK_LIBRARIES(levelset_seg_gui bvis1 dbgui vidpro1 vidl1 dbil_pro dbsol_pro dbsol_vis dbdet_pro dbacm_pro )


ENDIF (VXL_VGUI_FOUND)
