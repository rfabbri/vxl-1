# brcv/mvg/CMakeLists.txt
# Brown Multi-View Geometry Libraries.

include_directories( ${lems_SOURCE_DIR}/brcv/mvg )

# Brown VXL multi-view library
add_subdirectory( bmvl )

# Brown Camera Calibration Library
#add_subdirectory( dbcal )

# Photogrammetry Library (moved to VXD)
# add_subdirectory( dvpgl )

# Brown Epipolar Curve Library (moved to VXD)
# add_subdirectory( dbecl )

# Brown Video Registration Library
add_subdirectory( dbvrl )

# Brown Video Reconstruction Library
add_subdirectory( dbrct )

# Brown Stereo Rectification Library
add_subdirectory( dbrkf )

# Brown Information-theoretic Registration Library
add_subdirectory( dbirl )

# Brown Registered Camera Computation Library
# 
# TODO This will not compile with latest VXL because vgel has been terminated
# There is a copy of it in lemsvxl/old/contrib/gel/vgel
# If you need this, you must first modernize vgel and make it build
add_subdirectory( dbrcl )

# Brown Multiview Differential Geometry Library (moved to vxd)
# add_subdirectory( dbdif )
