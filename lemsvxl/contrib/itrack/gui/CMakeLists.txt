# contrib/itrack/gui/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${LEMS_SOURCE_DIR}/contrib/ )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )


set(itrack_gui_sources
   itrack_menu.cxx       itrack_menu.h
   itrack_main.cxx
)


aux_source_directory(Templates bvis1_sources)

add_executable(itrack_gui ${itrack_gui_sources})

target_link_libraries(itrack_gui dbinfo_vis dbinfo_pro dbinfo dbbgm_vis dbbgm_pro dbbgm bvis1 dbgui vidpro1 vsol vidl1)


endif (VXL_VGUI_FOUND)
