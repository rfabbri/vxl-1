# bbasd/bgld/tests/CMakeLists.txt


add_executable( bgld_test_all
  test_driver.cxx
  test_arc.cxx
  test_poly_line.cxx
  test_eno_curve.cxx
  test_eno_curve_3d.cxx
  test_linear_curve_3d.cxx
  test_area.cxx
)

target_link_libraries( bgld_test_all bgld testlib )

add_test( NAME  bgld_test_arc        COMMAND $<TARGET_FILE:bgld_test_all> test_arc )
add_test( NAME  bgld_test_poly_line  COMMAND $<TARGET_FILE:bgld_test_all> test_poly_line )
add_test( NAME  bgld_test_eno_curve  COMMAND $<TARGET_FILE:bgld_test_all> test_eno_curve )
add_test( NAME  bgld_test_eno_curve_3d  COMMAND $<TARGET_FILE:bgld_test_all> test_eno_curve_3d )
add_test( NAME  bgld_test_linear_curve_3d  COMMAND $<TARGET_FILE:bgld_test_all> test_linear_curve_3d )
add_test( NAME  bgld_test_area  COMMAND $<TARGET_FILE:bgld_test_all> test_area )


add_executable( bgld_test_include test_include.cxx )
target_link_libraries( bgld_test_include bgld vsl )
