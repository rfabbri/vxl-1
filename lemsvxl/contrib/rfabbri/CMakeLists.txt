# contrib/lems_rfabbri/CMakeLists.txt
# Developement for : lems_rfabbri

include_directories( ${lems_SOURCE_DIR}/algo )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )

# scrap bvis-based gui
add_subdirectory( gui )

# multiview stuff
add_subdirectory( mw )

# my command line executables
#add_subdirectory( cmd )
