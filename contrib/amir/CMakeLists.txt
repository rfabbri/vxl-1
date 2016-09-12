# contrib/amir/CMakeLists.txt
# Developement for : amir

include_directories( ${lems_SOURCE_DIR}/breye )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/contrib/amir )

# my bvis based GUI
#subdirs( gui )

# my command line executables
subdirs( cmd )



