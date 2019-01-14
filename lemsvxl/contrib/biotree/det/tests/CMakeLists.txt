# contrib/biotree/det/tests/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/contrib ${VXL_GEL_INCLUDE_DIR})
include_directories( ${LEMS_SOURCE_DIR}/db3p )

add_executable( det_test_all   
  test_driver.cxx
#  test_det_field_stats.cxx
  test_cylinder_map.cxx
  test_det_nonmaxium_suppression.cxx
  test_edge_det_nonmaximum_suppression.cxx
  test_det_nonmax_sup_helper.cxx
  test_det_suppression_and_interpolation.cxx
#  test_det_suppression_and_interpolation_2.cxx
)

target_link_libraries(det_test_all biodet vsol vbl testlib vcl )

#add_test( NAME  det_test_field_stats   COMMAND $<TARGET_FILE:det_test_all> test_det_field_stats)
add_test( NAME  det_test_cylinder_map  COMMAND $<TARGET_FILE:det_test_all> test_det_cylinder_map)
add_test( NAME  det_test_nonmax_sup_helper COMMAND $<TARGET_FILE:det_test_all> test_det_nonmax_sup_helper)
add_test( NAME  det_test_nonmaxium_suppression COMMAND $<TARGET_FILE:det_test_all> test_det_nonmaxium_suppression)
add_test( NAME  det_edge_test_nonmaximum_suppression COMMAND $<TARGET_FILE:det_test_all> test_edge_det_nonmaximum_suppression)

add_executable( det_test_include test_include.cxx )
target_link_libraries( det_test_include biodet )
