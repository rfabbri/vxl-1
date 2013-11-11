# This is contrib/biotree/biolung/pro/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

SET(biolung_process_sources
   biolung_process.cxx          biolung_process.h
)

ADD_LIBRARY(biolung_process ${biolung_process_sources})

TARGET_LINK_LIBRARIES(biolung_process biolung vidpro1)

IF(BUILD_TESTING)
#  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
