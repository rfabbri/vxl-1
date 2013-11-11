# This is basic/dbspi/tests

ADD_EXECUTABLE ( dbspi_test_all
  test_driver.cxx
  test_curve_distance.cxx
)

TARGET_LINK_LIBRARIES( dbspi_test_all dbspi  dbil_algo testlib )

ADD_TEST( dbspi_test_curve_distance dbspi_test_all test_curve_distance )

ADD_EXECUTABLE( dbspi_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbspi_test_include dbspi)
