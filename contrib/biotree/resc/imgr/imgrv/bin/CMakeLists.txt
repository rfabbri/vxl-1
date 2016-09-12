# biotree/resc/imgr/imgrv/bin/CMakeLists.txt

include_directories( ${VXLCORE_INCLUDE_DIR}
${biotree_SOURCE_DIR}/resc/imgr/file_formats )

set(view3d_sources view3d.cxx view3d_menus.cxx view3d_menus.h)

add_executable (view3d ${view3d_sources})

target_link_libraries(view3d imgrv vgui)
