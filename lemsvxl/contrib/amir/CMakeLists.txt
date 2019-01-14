# contrib/amir/CMakeLists.txt
# Developement for : amir

include_directories( ${LEMS_SOURCE_DIR}/breye )
include_directories( ${LEMS_SOURCE_DIR}/basic )
include_directories( ${LEMS_SOURCE_DIR}/contrib/amir )

# my bvis based GUI
#add_subdirectory( gui )

# my command line executables
add_subdirectory( cmd )



