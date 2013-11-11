# contrib/lems_itrack/CMakeLists.txt
# Developement for : lems_itrack

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/algo )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/breye2 )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/brcv )

# bvis based GUIS
SUBDIRS( gui2 )


# tests 
IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
