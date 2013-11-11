# contrib\edubrito\calibration

# Use VPGL to calibrate perspective cameras based on 3D world to image correspondences


INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bmvl )



ADD_EXECUTABLE( calibration_vpgl  calibration_vpgl.cxx)

TARGET_LINK_LIBRARIES( calibration_vpgl brct vpgl_algo vsol )
