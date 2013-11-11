# basic/dbsta/algo/tests/CMakeLists.txt
# Tests for dbsta_algo library

ADD_EXECUTABLE( dbsta_algo_test_all
  test_driver.cxx
  test_gaussian_stats.cxx
  test_adaptive_updater.cxx
  test_sinkhorn.cxx
)

TARGET_LINK_LIBRARIES( dbsta_algo_test_all dbsta_algo testlib )

ADD_TEST( dbsta_test_gaussian_stats           dbsta_algo_test_all test_gaussian_stats )
ADD_TEST( dbsta_test_adaptive_updater         dbsta_algo_test_all test_adaptive_updater )
ADD_TEST( dbsta_test_sinkhorn                 dbsta_algo_test_all test_sinkhorn )


ADD_EXECUTABLE( dbsta_algo_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( dbsta_algo_test_include dbsta_algo )
