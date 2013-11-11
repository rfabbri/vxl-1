# basic/dbil/algo/tests/CMakeLists.txt


ADD_EXECUTABLE( dbil_algo_test_all
  test_driver.cxx
  test_exact_distance_transform.cxx
)

TARGET_LINK_LIBRARIES( dbil_algo_test_all dbil_algo testlib )

ADD_TEST( dbil_algo_test_exact_distance_transform dbil_algo_test_all test_exact_distance_transform )


ADD_EXECUTABLE( dbil_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbil_algo_test_include dbil_algo )
