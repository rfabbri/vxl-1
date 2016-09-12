# contrib/shock2d/CMakeLists.txt
# Developement for : shock computation stuff

include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )

# my bvis1 based GUI
subdirs( gui )

# my command line executables
#subdirs( cmd )
