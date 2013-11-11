# Brown curve tracking library
# brcv/mvg/dbrkf/CMakeLists.txt
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES(${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${VXL_MUL_INCLUDE_DIR} )

SET(dbrkf_sources
   dbrkf_simplerect.cxx dbrkf_simplerect.h
)
AUX_SOURCE_DIRECTORY(Templates dbrkf_sources)

ADD_LIBRARY(dbrkf  ${dbrkf_sources})



TARGET_LINK_LIBRARIES(dbrkf vbl vdgl vtol vsol vil vnl  btol mvl mbl brct bdgl vsl)

IF(BUILD_TESTING)
 SUBDIRS(tests)
ENDIF(BUILD_TESTING)
