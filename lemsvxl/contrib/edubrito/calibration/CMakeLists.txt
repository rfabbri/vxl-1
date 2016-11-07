# contrib\edubrito\calibration

# Use VPGL to calibrate perspective cameras based on 3D world to image correspondences


include_directories(${VXL_BRL_INCLUDE_DIR}/bbas )
include_directories(${VXL_GEL_INCLUDE_DIR} )
include_directories(${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories(${VXL_BRL_INCLUDE_DIR}/bmvl )



add_executable( calibration_vpgl  calibration_vpgl.cxx)

target_link_libraries( calibration_vpgl brct vpgl_algo vsol )
