# contrib/lems_rfabbri/CMakeLists.txt
# Developement for : lems_rfabbri

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/algo )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

# scrap bvis-based gui
SUBDIRS( gui )

# multiview stuff
SUBDIRS( mw )

# my command line executables
#SUBDIRS( cmd )
