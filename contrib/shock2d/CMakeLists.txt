# contrib/shock2d/CMakeLists.txt
# Developement for : shock computation stuff

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

# my bvis1 based GUI
SUBDIRS( gui )

# my command line executables
#SUBDIRS( cmd )
