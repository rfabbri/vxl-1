# basic/dbnl/tests/CMakeLists.txt
# Tests for dbnl library

ADD_EXECUTABLE( dbnl_test_all
  test_driver.cxx
  test_fresnel.cxx
  test_sinc.cxx
  test_newton_root.cxx
  test_angle.cxx
  test_solve_quadratic_equation.cxx
  test_solve_trigonometric_equation.cxx
)

TARGET_LINK_LIBRARIES( dbnl_test_all dbnl testlib )

ADD_TEST( dbnl_test_fresnel              dbnl_test_all test_fresnel )
ADD_TEST( dbnl_test_sinc                 dbnl_test_all test_sinc )
ADD_TEST( dbnl_test_newton_root          dbnl_test_all test_newton_root )
ADD_TEST( dbnl_test_angle                dbnl_test_all test_angle )
ADD_TEST( dbnl_test_solve_quadratic_equation           dbnl_test_all    test_solve_quadratic_equation )
ADD_TEST( dbnl_test_solve_trigonometric_equation       dbnl_test_all    test_solve_trigonometric_equation )


ADD_EXECUTABLE( dbnl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbnl_test_include dbnl )
