# contrib/biotree/det/tests/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/contrib ${VXL_GEL_INCLUDE_DIR})
include_directories( ${lems_SOURCE_DIR}/db3p )

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

#add_test( det_test_field_stats   det_test_all test_det_field_stats)
add_test( det_test_cylinder_map  det_test_all test_det_cylinder_map)
add_test( det_test_nonmax_sup_helper det_test_all test_det_nonmax_sup_helper)
add_test( det_test_nonmaxium_suppression det_test_all test_det_nonmaxium_suppression)
add_test( det_edge_test_nonmaximum_suppression det_test_all test_edge_det_nonmaximum_suppression)

add_executable( det_test_include test_include.cxx )
target_link_libraries( det_test_include biodet )
