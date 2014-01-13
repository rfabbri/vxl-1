# contrib/lems_mauro/CMakeLists.txt
# Developement for : lems_mauro

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/algo )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

# my bvis based GUI
SUBDIRS( gui )

# my command line executables
SUBDIRS( cmd )
