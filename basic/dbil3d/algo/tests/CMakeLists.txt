# basic/dbil3d/algo/tests/CMakeLists.txt


ADD_EXECUTABLE( dbil3d_algo_test_all
  test_driver.cxx
  test_dbil3d_finite_differences.cxx
  test_dbil3d_finite_second_differences.cxx
)

TARGET_LINK_LIBRARIES( dbil3d_algo_test_all dbil3d_algo vil3d vil3d_io testlib )

ADD_TEST( dbil3d_algo_test_dbil3d_finite_differences          dbil3d_algo_test_all test_dbil3d_finite_differences )
ADD_TEST( dbil3d_algo_test_dbil3d_finite_second_differences          dbil3d_algo_test_all test_dbil3d_finite_second_differences )

ADD_EXECUTABLE( dbil3d_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbil3d_algo_test_include dbil3d_algo )
