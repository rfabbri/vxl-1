# brl/vvid/bin/CMakeLists.txt

#include_directories( ${VXLCORE_INCLUDE_DIR} )
include_directories(${VXL_GEL_INCLUDE_DIR}/mrc)
include_directories( ${lems_SOURCE_DIR}/contrib )

if(DIRECTSHOW_FOUND)
 set(videx_dshow_sources videx_dshow.cxx videx_dshow_menus.cxx videx_dshow_menus.h)
 add_executable (videx_dshow ${videx_dshow_sources})
 target_link_libraries(videx_dshow ozgevvid vidl_vil1 vgui vidl)
endif(DIRECTSHOW_FOUND)
