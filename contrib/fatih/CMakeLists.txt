# contrib/lems_fatih/CMakeLists.txt
# Developement for : lems_fatih

project(fatih_contrib)

#include_directories( ${lems_SOURCE_DIR}/algo )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/contrib/fatih )

# my command line executables
subdirs( cmd )
