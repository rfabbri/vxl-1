# This is lemsvxlsrc/bbasd/bnld/algo/tests

add_executable ( bnld_algo_test_all
   # Driver source and utilities
   test_driver.cxx

   # The tests
   test_eno_poly.cxx
   test_eno_zerox.cxx
   test_eno_image.cxx
   test_eno_interp.cxx
   test_eno_1d.cxx
   test_eno_shock_1d.cxx
   test_eno_third_order.cxx
   test_brent_root.cxx
)

target_link_libraries( bnld_algo_test_all bnld_algo testlib )

add_test( NAME  bnld_algo_test_eno_poly         COMMAND $<TARGET_FILE:bnld_algo_test_all> test_eno_poly )
add_test( NAME  bnld_algo_test_eno_interp       COMMAND $<TARGET_FILE:bnld_algo_test_all> test_eno_interp)
add_test( NAME  bnld_algo_test_eno_zerox        COMMAND $<TARGET_FILE:bnld_algo_test_all> test_eno_zerox )
add_test( NAME  bnld_algo_test_eno_1d           COMMAND $<TARGET_FILE:bnld_algo_test_all> test_eno_1d)
add_test( NAME  bnld_algo_test_eno_image        COMMAND $<TARGET_FILE:bnld_algo_test_all> test_eno_image)
add_test( NAME  bnld_algo_test_eno_shock_1d     COMMAND $<TARGET_FILE:bnld_algo_test_all> test_eno_shock_1d)
add_test( NAME  bnld_algo_test_eno_third_order  COMMAND $<TARGET_FILE:bnld_algo_test_all> test_eno_third_order)


add_executable( bnld_algo_test_include test_include.cxx )
target_link_libraries( bnld_algo_test_include bnld_algo )

