# dbxml/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/basic )

INCLUDE_DIRECTORIES(${VXL_GEL_INCLUDE_DIR} )

ADD_EXECUTABLE(dbxml_example dbxml_example.cxx)

TARGET_LINK_LIBRARIES(dbxml_example dbxml ${XERCES_LIBRARIES})

