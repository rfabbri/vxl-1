# bbasd/bnld/tests/CMakeLists.txt
# Tests for bnld library

add_executable( bnld_test_all
  test_driver.cxx
  test_fresnel.cxx
  test_sinc.cxx
  test_newton_root.cxx
  test_angle.cxx
  test_solve_quadratic_equation.cxx
  test_solve_trigonometric_equation.cxx
)

target_link_libraries( bnld_test_all bnld testlib )

add_test( NAME  bnld_test_fresnel              COMMAND $<TARGET_FILE:bnld_test_all> test_fresnel )
add_test( NAME  bnld_test_sinc                 COMMAND $<TARGET_FILE:bnld_test_all> test_sinc )
add_test( NAME  bnld_test_newton_root          COMMAND $<TARGET_FILE:bnld_test_all> test_newton_root )
add_test( NAME  bnld_test_angle                COMMAND $<TARGET_FILE:bnld_test_all> test_angle )
add_test( NAME  bnld_test_solve_quadratic_equation           COMMAND $<TARGET_FILE:bnld_test_all>    test_solve_quadratic_equation )
add_test( NAME  bnld_test_solve_trigonometric_equation       COMMAND $<TARGET_FILE:bnld_test_all>    test_solve_trigonometric_equation )


add_executable( bnld_test_include test_include.cxx )
target_link_libraries( bnld_test_include bnld )
