# This is brcv/dbmsh3d/examples/CMakeLists.txt
# Command-line programs for meshes
# 


INCLUDE_DIRECTORIES ( ${lems_SOURCE_DIR}/brcv/shp )

ADD_EXECUTABLE(mesh2ply dbmsh3d_mesh2ply.cxx)
TARGET_LINK_LIBRARIES( mesh2ply dbmsh3d_algo vul )

ADD_EXECUTABLE( dbmsh3d_example_triangles example_triangles.cxx )
TARGET_LINK_LIBRARIES( dbmsh3d_example_triangles dbmsh3d dbmsh3d_algo)
