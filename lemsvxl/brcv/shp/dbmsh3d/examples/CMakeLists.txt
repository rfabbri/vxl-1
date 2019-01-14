# This is brcv/dbmsh3d/examples/CMakeLists.txt
# Command-line programs for meshes
# 


include_directories ( ${LEMS_SOURCE_DIR}/brcv/shp )

add_executable(mesh2ply dbmsh3d_mesh2ply.cxx)
target_link_libraries( mesh2ply dbmsh3d_algo vul )

add_executable( dbmsh3d_example_triangles example_triangles.cxx )
target_link_libraries( dbmsh3d_example_triangles dbmsh3d dbmsh3d_algo)
