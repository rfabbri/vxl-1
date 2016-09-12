# brcv/mvg/CMakeLists.txt
# Brown Multi-View Geometry Libraries.

include_directories( ${lems_SOURCE_DIR}/brcv/mvg )

# Brown VXL multi-view library
add_subdirectory( bmvl )

# Brown Camera Calibration Library
#add_subdirectory( dbcal )

# Photogrammetry Library
add_subdirectory( dvpgl )

# Brown Epipolar Curve Library
add_subdirectory( dbecl )

# Brown Video Registration Library
add_subdirectory( dbvrl )

# Brown Video Reconstruction Library
add_subdirectory( dbrct )

# Brown Stereo Rectification Library
add_subdirectory( dbrkf )

# Brown Information-theoretic Registration Library
add_subdirectory( dbirl )

# Brown Registered Camera Computation Library
add_subdirectory( dbrcl )

# Brown Multiview Differential Geometry Library
add_subdirectory( dbdif )
