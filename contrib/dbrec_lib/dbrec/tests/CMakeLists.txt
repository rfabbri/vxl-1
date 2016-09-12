# lemsvxlsrc/contrib/dbrec_lib/dbrec/tests/CMakeLists.txt

add_executable( dbrec_test_all
  test_driver.cxx
  test_part.cxx
  test_primitive_factory.cxx
  test_type_id_factory.cxx
  test_part_context.cxx
  test_gaussian_primitive.cxx
  test_hierarchy.cxx
  test_compositor.cxx
  test_image_learner_visitors.cxx
  test_visitors.cxx
  test_models.cxx
  test_part_selector.cxx
  test_rot_inv.cxx
)

target_link_libraries( dbrec_test_all dbrec testlib )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_vehicle.png    ${CMAKE_CURRENT_BINARY_DIR}/test_vehicle.png COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_vehicle_mask.png    ${CMAKE_CURRENT_BINARY_DIR}/test_vehicle_mask.png COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gaussian_2_2_0_bright_fg_params_dbrec.txt ${CMAKE_CURRENT_BINARY_DIR}/gaussian_2_2_0_bright_fg_params_dbrec.txt COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gaussian_2_1_0_bright_fg_params_dbrec.txt ${CMAKE_CURRENT_BINARY_DIR}/gaussian_2_1_0_bright_fg_params_dbrec.txt COPYONLY )


add_test( dbrec_test_part  dbrec_test_all test_part )
add_test( dbrec_test_primitive_factory  dbrec_test_all test_primitive_factory )
add_test( dbrec_test_type_id_factory  dbrec_test_all test_type_id_factory )
add_test( dbrec_test_part_context  dbrec_test_all test_part_context )
add_test( dbrec_test_gaussian_primitive  dbrec_test_all test_gaussian_primitive )
add_test( dbrec_test_hierarchy  dbrec_test_all test_hierarchy )
add_test( dbrec_test_compositor  dbrec_test_all test_compositor )
add_test( dbrec_test_image_learner_visitors dbrec_test_all test_image_learner_visitors )
add_test( dbrec_test_visitors dbrec_test_all test_visitors )
add_test( dbrec_test_models dbrec_test_all test_models )
add_test( dbrec_test_part_selector  dbrec_test_all test_part_selector )
add_test( dbrec_test_rot_inv  dbrec_test_all test_rot_inv )

add_executable( dbrec_test_include test_include.cxx )
target_link_libraries( dbrec_test_include dbrec )
