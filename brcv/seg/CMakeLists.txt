# brcv/seg/CMakeLists.txt
# Brown Segmentation Libraries.

include_directories( ${lems_SOURCE_DIR}/brcv/seg )
include_directories( ${lems_BINARY_DIR}/brcv/seg )

# Brown Detection Library 
subdirs( dbdet )

# Brown Clustering Library 
subdirs( dbcll )

# Brown Background Modeling Library 
subdirs( dbbgm )

# Brown Active Contour Models Library
subdirs( dbacm )

# Brown 3D Active Contour Models Library
subdirs( dbacm3d )

# Brown Shock-Based Segmentation Library
subdirs( dbsks )


