# contrib/edge_det/CMakeLists.txt
# Developement for : edge detection and edge linking

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/contrib )

# my bvis1 based GUI
subdirs( gui )

# CSA library
subdirs( csa )

# local processes
subdirs( pro )

# local visualization routines
subdirs( vis )

