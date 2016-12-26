# /basic/dbxml/CMakeLists.txt
# Development Brown XML Library

include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${VXL_BRL_INCLUDE_DIR} )

set(dbxml_sources 
    dbxml_algos.h                 dbxml_algos.cxx
    dbxml_xio.h                   dbxml_xio.cxx
    dbxml_vector_xio.h            dbxml_vector_xio.hxx
)

aux_source_directory(Templates dbxml_sources)

add_library(dbxml  ${dbxml_sources})

target_link_libraries(dbxml bxml)

if( BUILD_TESTING )
#  add_subdirectory(tests)
endif( BUILD_TESTING )

