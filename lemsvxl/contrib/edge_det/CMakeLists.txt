# contrib/edge_det/CMakeLists.txt
# Developement for : edge detection and edge linking

include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/contrib )
include_directories(${VXD_BRLD_INCLUDE_DIR})

# my bvis1 based GUI
add_subdirectory( gui )

# CSA library
add_subdirectory( csa )

# local processes
add_subdirectory( pro )

# local visualization routines
add_subdirectory( vis )

