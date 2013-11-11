ADD_EXECUTABLE( dbrec3d_test_all
  test_driver.cxx
  test_parts_and_contexts.cxx
  test_non_maxima_suppression.cxx
  test_find_primitive_pairs.cxx
  test_visitors.cxx
  test_xml_io.cxx
)

SET(dbrec3d_test_all_sources
    test_utils.h    test_utils.cxx
    )

ADD_LIBRARY(dbrec3d_test_util  ${dbrec3d_test_all_sources})


TARGET_LINK_LIBRARIES( dbrec3d_test_all dbrec3d_test_util dbrec3d testlib)

ADD_TEST( test_parts_and_contexts      ${EXECUTABLE_OUTPUT_PATH}/dbrec3d_test_all   test_parts_and_contexts )
ADD_TEST( test_non_maxima_suppression  ${EXECUTABLE_OUTPUT_PATH}/dbrec3d_test_all   test_non_maxima_suppression )
ADD_TEST( test_find_primitive_pairs    ${EXECUTABLE_OUTPUT_PATH}/dbrec3d_test_all   test_find_primitive_pairs )
ADD_TEST( test_visitors                ${EXECUTABLE_OUTPUT_PATH}/dbrec3d_test_all   test_visitors )
ADD_TEST( test_xml_io                  ${EXECUTABLE_OUTPUT_PATH}/dbrec3d_test_all   test_xml_io )

ADD_EXECUTABLE( dbrec3d_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( dbrec3d_test_include dbrec3d)
