# biotree/resc/imgr/imgrv/bin/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR}
${biotree_SOURCE_DIR}/resc/imgr/file_formats )

SET(view3d_sources view3d.cxx view3d_menus.cxx view3d_menus.h)

ADD_EXECUTABLE (view3d ${view3d_sources})

TARGET_LINK_LIBRARIES(view3d imgrv vgui)
