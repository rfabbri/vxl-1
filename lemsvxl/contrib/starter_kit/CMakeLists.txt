# contrib/lems_starter_kit/CMakeLists.txt
# Developement for : lems_starter_kit

include_directories( ${LEMS_SOURCE_DIR}/algo )
include_directories( ${LEMS_SOURCE_DIR}/breye )
include_directories( ${LEMS_SOURCE_DIR}/basic )

# my bvis based GUI
add_subdirectory( gui )

# my command line executables
add_subdirectory( cmd )
