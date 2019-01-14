# contrib/shock2d/CMakeLists.txt
# Developement for : shock computation stuff

include_directories( ${LEMS_SOURCE_DIR}/breye1 )
include_directories( ${LEMS_SOURCE_DIR}/basic )

# my bvis1 based GUI
add_subdirectory( gui )

# my command line executables
#add_subdirectory( cmd )
