# brcv/mvg/CMakeLists.txt
# Brown Multi-View Geometry Libraries.

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv/mvg )

# Brown Camera Calibration Library
#SUBDIRS( dbcal )

# Photogrammetry Library
SUBDIRS( dvpgl )

# Brown Epipolar Curve Library
SUBDIRS( dbecl )

# Brown Video Registration Library
SUBDIRS( dbvrl )

# Brown Video Reconstruction Library
SUBDIRS( dbrct )

# Brown Stereo Rectification Library
SUBDIRS( dbrkf )

# Brown Information-theoretic Registration Library
SUBDIRS( dbirl )

# Brown Registered Camera Computation Library
SUBDIRS( dbrcl )

# Brown Multiview Differential Geometry Library
SUBDIRS( dbdif )
