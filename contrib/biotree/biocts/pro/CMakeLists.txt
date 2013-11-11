# This is contrib/biotree/biocts/pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(biocts_process_sources
   biocts_process.cxx          biocts_process.h
)

ADD_LIBRARY(biocts_process ${biocts_process_sources})

TARGET_LINK_LIBRARIES(biocts_process biocts vidpro1)

IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
