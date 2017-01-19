# brcv/CMakeLists.txt
# Brown Computer Vision Libraries.

include_directories( ${VXL_BRL_INCLUDE_DIR} )
include_directories( ${lems_SOURCE_DIR}/db3p )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories(${VXD_BRLD_INCLUDE_DIR})

# Brown Segmentation Libraries
add_subdirectory( seg )

# Brown Shape Libraries
add_subdirectory( shp )

# Brown Recognition Libraries
add_subdirectory( rec )

# Brown Tracking Libraries
add_subdirectory( trk )

# Brown Multi-View Geometry Libraries
add_subdirectory( mvg )



