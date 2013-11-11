# This is lemsvxlsrc/basic/dbnl/algo/tests

ADD_EXECUTABLE ( dbnl_algo_test_all
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

TARGET_LINK_LIBRARIES( dbnl_algo_test_all dbnl_algo testlib )

ADD_TEST( dbnl_algo_test_eno_poly         dbnl_algo_test_all test_eno_poly )
ADD_TEST( dbnl_algo_test_eno_interp       dbnl_algo_test_all test_eno_interp)
ADD_TEST( dbnl_algo_test_eno_zerox        dbnl_algo_test_all test_eno_zerox )
ADD_TEST( dbnl_algo_test_eno_1d           dbnl_algo_test_all test_eno_1d)
ADD_TEST( dbnl_algo_test_eno_image        dbnl_algo_test_all test_eno_image)
ADD_TEST( dbnl_algo_test_eno_shock_1d     dbnl_algo_test_all test_eno_shock_1d)
ADD_TEST( dbnl_algo_test_eno_third_order  dbnl_algo_test_all test_eno_third_order)


ADD_EXECUTABLE( dbnl_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbnl_algo_test_include dbnl_algo )

