# brcv/rec/CMakeLists.txt
# Brown Recognition Libraries.

include_directories( ${lems_SOURCE_DIR}/brcv/rec )

# Brown 3D Mesh Recognition Library
subdirs( dbmsh3dr )

# Brown 3D Shock Recognition Library
subdirs( dbsk3dr )
 
# Brown Graph Assignment Solvers
subdirs( dbasn )

project(rec)

# Brown Elastic Curve Matching Library
subdirs( dbcvr )

# Brown Keypoint Recognition Library
subdirs( dbkpr )

# Brown Mutual Information Recognition Library
subdirs( dbru )

# Brown 2D Shock Recognition Library
subdirs( dbskr )

# Brown Edge-Based Recognition Library
subdirs( dber )

# Brown ORL - Object Recognition Library - web database to store datasets and cluster management via MPI
subdirs( dborl )

# Brown Curve-based Spatial Indexing Library
subdirs( dbcsi )
