# brcv/seg/CMakeLists.txt
# Brown Segmentation Libraries.

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/seg )
INCLUDE_DIRECTORIES( ${lems_BINARY_DIR}/brcv/seg )

# Brown Detection Library 
SUBDIRS( dbdet )

# Brown Clustering Library 
SUBDIRS( dbcll )

# Brown Background Modeling Library 
SUBDIRS( dbbgm )

# Brown Active Contour Models Library
SUBDIRS( dbacm )

# Brown 3D Active Contour Models Library
SUBDIRS( dbacm3d )

# Brown Shock-Based Segmentation Library
SUBDIRS( dbsks )


