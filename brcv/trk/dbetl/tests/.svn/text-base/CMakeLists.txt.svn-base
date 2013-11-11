# algo/dbetl/tests/CMakeLists.txt


ADD_EXECUTABLE( dbetl_test_all
  test_driver.cxx
  test_image_stats.cxx
)

TARGET_LINK_LIBRARIES( dbetl_test_all dbetl testlib )

ADD_TEST( dbetl_image_stats            dbetl_test_all test_image_stats )

ADD_EXECUTABLE( dbetl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbetl_test_include dbetl )
