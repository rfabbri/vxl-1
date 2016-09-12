# brcv/CMakeLists.txt
# Brown Computer Vision Libraries.

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/basic )

# Brown Segmentation Libraries
subdirs( seg )

# Brown Shape Libraries
subdirs( shp )

# Brown Recognition Libraries
subdirs( rec )

# Brown Tracking Libraries
subdirs( trk )

# Brown Multi-View Geometry Libraries
subdirs( mvg )



