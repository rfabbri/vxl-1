# contrib/multi_view/gui2/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${LEMS_SOURCE_DIR}/contrib/ )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/brcv/mvg )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )

set(mview2_gui_sources
   mview2_menu.cxx       mview2_menu.h
   mview2_main.cxx
)


aux_source_directory(Templates bvis_sources)

add_executable(mview2_gui ${mview2_gui_sources})

target_link_libraries(mview2_gui dvpgl_vis dbinfo_vis dbinfo_pro dbinfo dbbgm_vis dbbgm_pro dbbgm bvis dbgui vidpro vsol vidl1 dbpro)


endif (VXL_VGUI_FOUND)
