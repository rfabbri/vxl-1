# contrib/biotree/gui/CMakeLists.txt


if (VXL_VGUI_FOUND)
if (BUILD_BREYE)
include_directories( ${lems_SOURCE_DIR}/contrib )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

set(biotree_gui_sources
   biotree_menu.cxx       biotree_menu.h
   biotree_main.cxx
)


aux_source_directory(Templates bvis1_sources)

add_executable(biotree_gui ${biotree_gui_sources})

target_link_libraries(biotree_gui bvis1 biolung_process biocts_process)
endif (BUILD_BREYE)
endif (VXL_VGUI_FOUND)
