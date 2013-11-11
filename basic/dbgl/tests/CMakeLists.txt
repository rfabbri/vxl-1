# basic/dbgl/tests/CMakeLists.txt


ADD_EXECUTABLE( dbgl_test_all
  test_driver.cxx
  test_arc.cxx
  test_poly_line.cxx
  test_eno_curve.cxx
  test_minimal_container.cxx
  test_eno_curve_3d.cxx
  test_linear_curve_3d.cxx
  test_area.cxx
)

TARGET_LINK_LIBRARIES( dbgl_test_all dbgl testlib )

ADD_TEST( dbgl_test_arc        dbgl_test_all test_arc )
ADD_TEST( dbgl_test_poly_line  dbgl_test_all test_poly_line )
ADD_TEST( dbgl_test_eno_curve  dbgl_test_all test_eno_curve )
ADD_TEST( dbgl_test_minimal_container  dbgl_test_all test_minimal_container )
ADD_TEST( dbgl_test_eno_curve_3d  dbgl_test_all test_eno_curve_3d )
ADD_TEST( dbgl_test_linear_curve_3d  dbgl_test_all test_linear_curve_3d )
ADD_TEST( dbgl_test_area  dbgl_test_all test_area )


ADD_EXECUTABLE( dbgl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbgl_test_include dbgl vsl )
