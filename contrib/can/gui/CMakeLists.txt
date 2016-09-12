# contrib/can/gui/CMakeLists.txt


if (VXL_VGUI_FOUND)

include_directories( ${lems_SOURCE_DIR}/basic )
#include_directories( ${lems_SOURCE_DIR}/contrib )
include_directories( ${lems_SOURCE_DIR}/breye )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR} )

set(can_gui_sources
   can_menu.cxx       can_menu.h
   can_main.cxx
)


aux_source_directory(Templates bvis_sources)

add_executable(can_gui ${can_gui_sources})

target_link_libraries(can_gui bvis dbil_pro)


endif (VXL_VGUI_FOUND)
