# shp/dbsksp/tests/CMakeLists.txt


ADD_EXECUTABLE( dbsksp_test_all
  test_driver.cxx
  test_shock_graph.cxx
  test_shapelet.cxx
  test_xshock_fragment.cxx
)

TARGET_LINK_LIBRARIES( dbsksp_test_all dbsksp testlib )

ADD_TEST( dbsksp_test_xshock_fragment  dbsksp_test_all test_xshock_fragment )


ADD_EXECUTABLE( dbsksp_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbsksp_test_include dbsksp vsl )
