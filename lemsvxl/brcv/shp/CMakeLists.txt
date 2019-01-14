# brcv/shp/CMakeLists.txt
# Brown Shape Libraries.

include_directories( ${LEMS_SOURCE_DIR}/brcv/shp )

# Brown Shock 2D Library
add_subdirectory( dbsk2d )

# Brown 3D Point Cloud Library
add_subdirectory( dbmsh3d )

# Brown Mesh Library
add_subdirectory( dbgdt3d )

# Brown Shock 3D Library
add_subdirectory( dbsk3d )

# Brown Shape-from-Shock library
add_subdirectory( dbsksp )

