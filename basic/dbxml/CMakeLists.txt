# /basic/dbxml/CMakeLists.txt
# Development Brown XML Library

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR} )

SET(dbxml_sources 
    dbxml_algos.h                 dbxml_algos.cxx
    dbxml_xio.h                   dbxml_xio.cxx
    dbxml_vector_xio.h            dbxml_vector_xio.txx
)

AUX_SOURCE_DIRECTORY(Templates dbxml_sources)

ADD_LIBRARY(dbxml  ${dbxml_sources})

TARGET_LINK_LIBRARIES(dbxml bxml)

IF( BUILD_TESTING )
#  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

