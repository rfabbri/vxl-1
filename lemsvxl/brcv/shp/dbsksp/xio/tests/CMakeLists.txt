# shp/dbsksp/xio/tests/CMakeLists.txt


add_executable( dbsksp_xio_test_all
  test_driver.cxx
  test_shock_graph_xio.cxx
)

target_link_libraries( dbsksp_xio_test_all dbsksp_xio testlib )

# add_test( dbsksp_xio_test_shock_graph   dbsksp_xio_test_all test_shock_graph )


add_executable( dbsksp_xio_test_include test_include.cxx )
target_link_libraries( dbsksp_xio_test_include dbsksp_xio )

