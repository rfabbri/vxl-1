# This is contrib/dec/hsds/CMakeLists.txt
# Hierachical Spatial Data Structures (quadtrees, octrees, etc)



SET(hsds_sources
    hsds_fd_tree_node_index.h
    hsds_fd_tree.h hsds_fd_tree.txx
    hsds_fd_tree_incremental_reader.h hsds_fd_tree_incremental_reader.txx
   )

AUX_SOURCE_DIRECTORY(Templates hsds_sources)

ADD_LIBRARY(hsds ${hsds_sources})

TARGET_LINK_LIBRARIES(hsds vnl vgl vbl)

SUBDIRS(tests)





