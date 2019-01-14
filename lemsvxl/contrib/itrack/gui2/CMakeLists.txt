# contrib/itrack2/gui/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${VXL_GEL_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${LEMS_SOURCE_DIR}/contrib/ )
include_directories( ${LEMS_SOURCE_DIR}/brcv/trk )
include_directories( ${LEMS_SOURCE_DIR}/brcv/seg )

set(itrack2_gui_sources
   itrack2_menu.cxx       itrack2_menu.h
   itrack2_main.cxx
)


aux_source_directory(Templates bvis_sources)

add_executable(itrack2_gui ${itrack2_gui_sources})

target_link_libraries(itrack2_gui dbinfo_vis dbbgm_vis bvis dbgui vsol vidl1)


endif (VXL_VGUI_FOUND)
