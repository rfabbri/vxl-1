# contrib/lems_itrack/CMakeLists.txt
# Developement for : lems_itrack

include_directories( ${lems_SOURCE_DIR}/algo )
include_directories( ${lems_SOURCE_DIR}/breye )
include_directories( ${lems_SOURCE_DIR}/contrib/breye2 )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv )

# bvis based GUIS
add_subdirectory( gui2 )


# tests 
if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)
