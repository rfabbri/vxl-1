# This is /contrib/rfabbri/mw/tests

ADD_EXECUTABLE ( mw_test_all
   # Driver source and utilities
   test_driver.cxx

   # The tests
   test_mw_util.cxx
   test_mw_discrete_corresp_io.cxx
   test_mw_discrete_corresp_n_io.cxx
   test_dbmcs_curve_3d_sketch_io.cxx
)

TARGET_LINK_LIBRARIES( mw_test_all mw testlib )

ADD_TEST( mw_test_mw_util mw_test_all test_mw_util )
ADD_TEST( mw_test_discrete_corresp_io mw_test_all test_discrete_corresp_io )
ADD_TEST( mw_test_discrete_corresp_n_io mw_test_all test_discrete_corresp_n_io )
ADD_TEST( dbmcs_curve_3d_sketch_io mw_test_all test_dbmcs_curve_3d_sketch_io)

ADD_EXECUTABLE( mw_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( mw_test_include mw )
