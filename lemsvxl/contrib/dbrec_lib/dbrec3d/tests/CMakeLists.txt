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

add_test( NAME  test_parts_and_contexts      COMMAND $<TARGET_FILE:dbrec3d_test_all>   test_parts_and_contexts )
add_test( NAME  test_non_maxima_suppression  COMMAND $<TARGET_FILE:dbrec3d_test_all>   test_non_maxima_suppression )
add_test( NAME  test_find_primitive_pairs    COMMAND $<TARGET_FILE:dbrec3d_test_all>   test_find_primitive_pairs )
add_test( NAME  test_visitors                COMMAND $<TARGET_FILE:dbrec3d_test_all>   test_visitors )
add_test( NAME  test_xml_io                  COMMAND $<TARGET_FILE:dbrec3d_test_all>   test_xml_io )

add_executable( dbrec3d_test_include test_include.cxx )

target_link_libraries( dbrec3d_test_include dbrec3d)
