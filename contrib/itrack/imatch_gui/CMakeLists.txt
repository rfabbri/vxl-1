# contrib/itrack/imatch_gui/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${lems_SOURCE_DIR}/contrib/ )
include_directories( ${lems_SOURCE_DIR}/brcv/trk )
include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_SOURCE_DIR}/brcv/rec )
include_directories( ${lems_SOURCE_DIR}/breye1 )

set(imatch_gui_sources
   imatch_menu.cxx       imatch_menu.h
   imatch_main.cxx
)

add_executable(imatch_gui ${imatch_gui_sources})

target_link_libraries(imatch_gui dbinfo_vis dbinfo_pro dbinfo dbbgm_vis dbbgm_pro dbbgm bvis1 dbgui vidpro1 vsol vidl1)


endif (VXL_VGUI_FOUND)
