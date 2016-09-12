# contrib/lems_itrack/CMakeLists.txt
# Developement for : lems_itrack

include_directories( ${lems_SOURCE_DIR}/algo )
include_directories( ${lems_SOURCE_DIR}/breye )
include_directories( ${lems_SOURCE_DIR}/breye1 )
include_directories( ${lems_SOURCE_DIR}/basic )
include_directories( ${lems_SOURCE_DIR}/brcv )

# bvis based GUIS
subdirs( gui )
subdirs( gui2 )
subdirs( imatch_gui )

# command line executables
#subdirs( cmd )


# vidpro elements
subdirs( cmd )

# vidpro elements
#subdirs( pro )

# tests for itrack classes
if(BUILD_TESTING)
  subdirs(tests)
endif(BUILD_TESTING)
