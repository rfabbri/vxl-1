# brl/vvid/bin/CMakeLists.txt

#INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR}/mrc)
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )

IF(DIRECTSHOW_FOUND)
 SET(videx_dshow_sources videx_dshow.cxx videx_dshow_menus.cxx videx_dshow_menus.h)
 ADD_EXECUTABLE (videx_dshow ${videx_dshow_sources})
 TARGET_LINK_LIBRARIES(videx_dshow ozgevvid vidl_vil1 vgui vidl)
ENDIF(DIRECTSHOW_FOUND)
