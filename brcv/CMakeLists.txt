# brcv/CMakeLists.txt
# Brown Computer Vision Libraries.

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/db3p )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

# Brown Segmentation Libraries
SUBDIRS( seg )

# Brown Shape Libraries
SUBDIRS( shp )

# Brown Recognition Libraries
SUBDIRS( rec )

# Brown Tracking Libraries
SUBDIRS( trk )

# Brown Multi-View Geometry Libraries
SUBDIRS( mvg )



