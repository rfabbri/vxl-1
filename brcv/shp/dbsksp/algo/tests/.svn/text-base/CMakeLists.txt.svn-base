# This is shp/dbsksp/algo/tests/CMakeLists.txt


ADD_EXECUTABLE( dbsksp_algo_test_all
  test_driver.cxx
  test_matching_xshock_directed_tree.cxx
  test_fit_xgraph.cxx
  test_interp_xshock_fragment.cxx
)
TARGET_LINK_LIBRARIES( dbsksp_algo_test_all dbsksp_algo dbsksp_xio testlib dbtest )

# Tests to go to the dashboard
ADD_TEST( dbsksp_algo_test_matching_xshock_directed_tree  dbsksp_algo_test_all test_matching_xshock_directed_tree )
ADD_TEST( dbsksp_algo_test_fit_xgraph dbsksp_algo_test_all test_fit_xgraph )
ADD_TEST( dbsksp_algo_test_interp_xshock_fragment dbsksp_algo_test_all test_interp_xshock_fragment )


ADD_EXECUTABLE( dbsksp_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbsksp_algo_test_include dbsksp_algo )
