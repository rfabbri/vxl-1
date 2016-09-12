# contrib/lems_itrack/CMakeLists.txt
# Developement for : lems_itrack

include_directories( ${lems_SOURCE_DIR}/algo )
include_directories( ${lems_SOURCE_DIR}/breye )
include_directories( ${lems_SOURCE_DIR}/contrib/breye2 )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv )

# bvis based GUIS
subdirs( gui2 )


# tests 
if(BUILD_TESTING)
  subdirs(tests)
endif(BUILD_TESTING)
