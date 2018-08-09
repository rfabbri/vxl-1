# basic/dbsta/algo/tests/CMakeLists.txt
# Tests for dbsta_algo library

add_executable( dbsta_algo_test_all
  test_driver.cxx
  test_gaussian_stats.cxx
  test_adaptive_updater.cxx
  test_sinkhorn.cxx
)

target_link_libraries( dbsta_algo_test_all dbsta_algo testlib )

add_test( NAME  dbsta_test_gaussian_stats           COMMAND $<TARGET_FILE:dbsta_algo_test_all> test_gaussian_stats )
add_test( NAME  dbsta_test_adaptive_updater         COMMAND $<TARGET_FILE:dbsta_algo_test_all> test_adaptive_updater )
add_test( NAME  dbsta_test_sinkhorn                 COMMAND $<TARGET_FILE:dbsta_algo_test_all> test_sinkhorn )


add_executable( dbsta_algo_test_include test_include.cxx )

target_link_libraries( dbsta_algo_test_include dbsta_algo )
