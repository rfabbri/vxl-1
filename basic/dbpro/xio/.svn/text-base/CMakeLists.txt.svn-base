# basic/dbpro/xio/CMakeLists.txt
# Development Brown Processing XML IO Library

INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bbas )

SET(dbpro_xio_sources

  dbpro_xio_source.cxx            dbpro_xio_source.h
  dbpro_xio_sink.cxx              dbpro_xio_sink.h

)


#AUX_SOURCE_DIRECTORY(Templates dbpro_xio_sources)

ADD_LIBRARY(dbpro_xio  ${dbpro_xio_sources})

TARGET_LINK_LIBRARIES(dbpro_xio dbpro bxml)

