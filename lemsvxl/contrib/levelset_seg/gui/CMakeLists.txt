# contrib/levelset_seg/gui/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

# local repository paths
include_directories( ${lems_SOURCE_DIR}/brcv/seg )

set(levelset_seg_gui_sources
   levelset_seg_menu.cxx          levelset_seg_menu.h
   levelset_seg_main.cxx
)


aux_source_directory(Templates bvis1_sources)

add_executable(levelset_seg_gui ${levelset_seg_gui_sources})

target_link_libraries(levelset_seg_gui bvis1 dbgui vidpro1 vidl1 dbil_pro dbsol_pro dbsol_vis dbdet_pro dbacm_pro )


endif (VXL_VGUI_FOUND)
