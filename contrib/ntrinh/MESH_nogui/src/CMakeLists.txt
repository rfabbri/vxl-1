project(MESH_nogui)

include_directories(${MESH_nogui_SOURCE_DIR} )

add_subdirectory(lib3d)
add_subdirectory(mesh_error_lib)
add_subdirectory(mesh_cmd)
