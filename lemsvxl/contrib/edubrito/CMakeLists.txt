# contrib/edubrito/CMakeLists.txt
# Developement for : edubrito

include_directories( ${LEMS_SOURCE_DIR}/algo )
include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/basic )

# camera calibration
add_subdirectory( calibration )
