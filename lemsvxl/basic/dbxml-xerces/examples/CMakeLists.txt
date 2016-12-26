# dbxml/tests/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/basic )

include_directories(${VXL_GEL_INCLUDE_DIR} )

add_executable(dbxml_example dbxml_example.cxx)

target_link_libraries(dbxml_example dbxml ${XERCES_LIBRARIES})

