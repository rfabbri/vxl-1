# brcv/seg/CMakeLists.txt
# Brown Segmentation Libraries.

include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_BINARY_DIR}/brcv/seg )

# Brown Detection Library 
add_subdirectory( dbdet )

# Brown Clustering Library 
add_subdirectory( dbcll )

# Brown Background Modeling Library 
add_subdirectory( dbbgm )

# Brown Active Contour Models Library
add_subdirectory( dbacm )

# Brown 3D Active Contour Models Library
add_subdirectory( dbacm3d )

# Brown Shock-Based Segmentation Library
add_subdirectory( dbsks )


