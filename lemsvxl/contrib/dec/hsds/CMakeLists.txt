# This is contrib/dec/hsds/CMakeLists.txt
# Hierachical Spatial Data Structures (quadtrees, octrees, etc)



set(hsds_sources
    hsds_fd_tree_node_index.h
    hsds_fd_tree.h hsds_fd_tree.hxx
    hsds_fd_tree_incremental_reader.h hsds_fd_tree_incremental_reader.hxx
   )

aux_source_directory(Templates hsds_sources)

add_library(hsds ${hsds_sources})

target_link_libraries(hsds vnl vgl vbl)

add_subdirectory(tests)





