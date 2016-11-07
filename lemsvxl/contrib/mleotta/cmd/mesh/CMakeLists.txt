# contrib/mleotta/cmd/mesh/CMakeLists.txt
# Command line programs for mesh computations

include_directories( ${VXL_GEL_INCLUDE_DIR} )

add_executable(mesh_subdivide mesh_subdivide.cxx)
target_link_libraries(mesh_subdivide imesh_algo vul)

add_executable(mesh_extract_features mesh_extract_features.cxx)
target_link_libraries(mesh_extract_features imesh_algo modrec vul)

add_executable(make_vehicle_mesh make_vehicle_mesh.cxx)
target_link_libraries(make_vehicle_mesh modrec imesh_algo vul)

add_executable(fit_mesh_body fit_mesh_body.cxx)
target_link_libraries(fit_mesh_body modrec imesh_algo vul)

add_executable(fit_parts fit_parts.cxx)
target_link_libraries(fit_parts modrec vul)

add_executable(parts2svg parts2svg.cxx)
target_link_libraries(parts2svg modrec vul)

add_executable(pca_fit_mesh_body pca_fit_mesh_body.cxx)
target_link_libraries(pca_fit_mesh_body imesh_algo vul)

add_executable(mesh_pca mesh_pca.cxx)
target_link_libraries(mesh_pca imesh_algo vul)

add_executable(parts_pca parts_pca.cxx)
target_link_libraries(parts_pca modrec vul)

add_executable(mesh_exterior mesh_exterior.cxx)
target_link_libraries(mesh_exterior imesh_algo vul)

