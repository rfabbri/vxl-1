# contrib/lems_itrack/CMakeLists.txt
# Developement for : lems_itrack

include_directories( ${lems_SOURCE_DIR}/algo )
include_directories( ${lems_SOURCE_DIR}/breye )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv )

# bvis based GUIS
add_subdirectory( gui )
add_subdirectory( gui2 )
add_subdirectory( imatch_gui )

# command line executables
#add_subdirectory( cmd )


# vidpro elements
add_subdirectory( cmd )

# vidpro elements
#add_subdirectory( pro )

# tests for itrack classes
if(BUILD_TESTING)
  add_subdirectory(tests)
endif(BUILD_TESTING)
