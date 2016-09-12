# brcv/mvg/CMakeLists.txt
# Brown Multi-View Geometry Libraries.

include_directories( ${lems_SOURCE_DIR}/brcv/mvg )

# Brown VXL multi-view library
subdirs ( bmvl )

# Brown Camera Calibration Library
#subdirs( dbcal )

# Photogrammetry Library
subdirs( dvpgl )

# Brown Epipolar Curve Library
subdirs( dbecl )

# Brown Video Registration Library
subdirs( dbvrl )

# Brown Video Reconstruction Library
subdirs( dbrct )

# Brown Stereo Rectification Library
subdirs( dbrkf )

# Brown Information-theoretic Registration Library
subdirs( dbirl )

# Brown Registered Camera Computation Library
subdirs( dbrcl )

# Brown Multiview Differential Geometry Library
subdirs( dbdif )
