# contrib/lems_starter_kit/CMakeLists.txt
# Developement for : lems_starter_kit

include_directories( ${lems_SOURCE_DIR}/algo )
include_directories( ${lems_SOURCE_DIR}/breye )
include_directories( ${lems_SOURCE_DIR}/basic )

# my bvis based GUI
subdirs( gui )

# my command line executables
subdirs( cmd )
