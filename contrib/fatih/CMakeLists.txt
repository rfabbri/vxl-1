# contrib/lems_fatih/CMakeLists.txt
# Developement for : lems_fatih

PROJECT(fatih_contrib)

#INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/algo )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/fatih )

# my command line executables
SUBDIRS( cmd )
