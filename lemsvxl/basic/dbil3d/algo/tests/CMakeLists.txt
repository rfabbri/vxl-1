# basic/dbil3d/algo/tests/CMakeLists.txt


add_executable( dbil3d_algo_test_all
  test_driver.cxx
  test_dbil3d_finite_differences.cxx
  test_dbil3d_finite_second_differences.cxx
)

target_link_libraries( dbil3d_algo_test_all dbil3d_algo vil3d vil3d_io testlib )

add_test( NAME  dbil3d_algo_test_dbil3d_finite_differences          COMMAND $<TARGET_FILE:dbil3d_algo_test_all> test_dbil3d_finite_differences )
add_test( NAME  dbil3d_algo_test_dbil3d_finite_second_differences          COMMAND $<TARGET_FILE:dbil3d_algo_test_all> test_dbil3d_finite_second_differences )

add_executable( dbil3d_algo_test_include test_include.cxx )
target_link_libraries( dbil3d_algo_test_include dbil3d_algo )
