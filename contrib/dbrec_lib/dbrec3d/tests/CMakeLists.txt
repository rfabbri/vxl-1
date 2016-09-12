add_executable( dbrec3d_test_all
  test_driver.cxx
  test_parts_and_contexts.cxx
  test_non_maxima_suppression.cxx
  test_find_primitive_pairs.cxx
  test_visitors.cxx
  test_xml_io.cxx
)

set(dbrec3d_test_all_sources
    test_utils.h    test_utils.cxx
    )

add_library(dbrec3d_test_util  ${dbrec3d_test_all_sources})


target_link_libraries( dbrec3d_test_all dbrec3d_test_util dbrec3d testlib)

add_test( test_parts_and_contexts      ${EXECUTABLE_OUTPUT_PATH}/dbrec3d_test_all   test_parts_and_contexts )
add_test( test_non_maxima_suppression  ${EXECUTABLE_OUTPUT_PATH}/dbrec3d_test_all   test_non_maxima_suppression )
add_test( test_find_primitive_pairs    ${EXECUTABLE_OUTPUT_PATH}/dbrec3d_test_all   test_find_primitive_pairs )
add_test( test_visitors                ${EXECUTABLE_OUTPUT_PATH}/dbrec3d_test_all   test_visitors )
add_test( test_xml_io                  ${EXECUTABLE_OUTPUT_PATH}/dbrec3d_test_all   test_xml_io )

add_executable( dbrec3d_test_include test_include.cxx )

target_link_libraries( dbrec3d_test_include dbrec3d)
