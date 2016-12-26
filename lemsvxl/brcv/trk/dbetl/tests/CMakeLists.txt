# algo/dbetl/tests/CMakeLists.txt


add_executable( dbetl_test_all
  test_driver.cxx
  test_image_stats.cxx
)

target_link_libraries( dbetl_test_all dbetl testlib )

add_test( dbetl_image_stats            dbetl_test_all test_image_stats )

add_executable( dbetl_test_include test_include.cxx )
target_link_libraries( dbetl_test_include dbetl )
