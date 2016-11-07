# brcv/rec/CMakeLists.txt
# Brown Recognition Libraries.

include_directories( ${lems_SOURCE_DIR}/brcv/rec )

# Brown 3D Mesh Recognition Library
add_subdirectory( dbmsh3dr )

# Brown 3D Shock Recognition Library
add_subdirectory( dbsk3dr )
 
# Brown Graph Assignment Solvers
add_subdirectory( dbasn )

project(rec)

# Brown Elastic Curve Matching Library
add_subdirectory( dbcvr )

# Brown Keypoint Recognition Library
add_subdirectory( dbkpr )

# Brown Mutual Information Recognition Library
add_subdirectory( dbru )

# Brown 2D Shock Recognition Library
add_subdirectory( dbskr )

# Brown Edge-Based Recognition Library
add_subdirectory( dber )

# Brown ORL - Object Recognition Library - web database to store datasets and cluster management via MPI
add_subdirectory( dborl )

# Brown Curve-based Spatial Indexing Library
add_subdirectory( dbcsi )
