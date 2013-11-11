# contrib/lems_itrack/CMakeLists.txt
# Developement for : lems_itrack

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/algo )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv )

# bvis based GUIS
SUBDIRS( gui )
SUBDIRS( gui2 )
SUBDIRS( imatch_gui )

# command line executables
#SUBDIRS( cmd )


# vidpro elements
SUBDIRS( cmd )

# vidpro elements
#SUBDIRS( pro )

# tests for itrack classes
IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
