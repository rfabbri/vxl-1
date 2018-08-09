# shp/dbsksp/tests/CMakeLists.txt


add_executable( dbsksp_test_all
  test_driver.cxx
  test_shock_graph.cxx
  test_shapelet.cxx
  test_xshock_fragment.cxx
)

target_link_libraries( dbsksp_test_all dbsksp testlib )

add_test( NAME  dbsksp_test_xshock_fragment  COMMAND $<TARGET_FILE:dbsksp_test_all> test_xshock_fragment )


add_executable( dbsksp_test_include test_include.cxx )
target_link_libraries( dbsksp_test_include dbsksp vsl )
