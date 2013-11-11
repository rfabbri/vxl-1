# This is rec/dbcsi/tests

ADD_EXECUTABLE ( dbcsi_test_all
  test_driver.cxx
  test_curve_distance.cxx
)

TARGET_LINK_LIBRARIES( dbcsi_test_all dbcsi dbdet_algo vil_algo testlib )

ADD_TEST( dbcsi_test_curve_distance dbcsi_test_all test_curve_distance )

ADD_EXECUTABLE( dbcsi_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbcsi_test_include dbcsi)
