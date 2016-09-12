# This is contrib/dec/hsds/CMakeLists.txt
# Hierachical Spatial Data Structures (quadtrees, octrees, etc)



set(hsds_sources
    hsds_fd_tree_node_index.h
    hsds_fd_tree.h hsds_fd_tree.txx
    hsds_fd_tree_incremental_reader.h hsds_fd_tree_incremental_reader.txx
   )

aux_source_directory(Templates hsds_sources)

add_library(hsds ${hsds_sources})

target_link_libraries(hsds vnl vgl vbl)

subdirs(tests)





