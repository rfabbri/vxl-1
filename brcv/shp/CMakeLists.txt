# brcv/shp/CMakeLists.txt
# Brown Shape Libraries.

include_directories( ${lems_SOURCE_DIR}/brcv/shp )

# Brown Shock 2D Library
subdirs( dbsk2d )

# Brown 3D Point Cloud Library
subdirs( dbmsh3d )

# Brown Mesh Library
subdirs( dbgdt3d )

# Brown Shock 3D Library
subdirs( dbsk3d )

# Brown Shape-from-Shock library
subdirs ( dbsksp )

