# seg/dbsks/tests/CMakeLists.txt
# Tests for dbsks library

ADD_EXECUTABLE( dbsks_xio_test_all
  test_driver.cxx
  test_xio_xshock.cxx
)
TARGET_LINK_LIBRARIES( dbsks_xio_test_all dbsks_xio testlib)

#ADD_TEST( dbsks_test_xio_xshock dbsks_test_all test_xio_xshock )

# Test include files
ADD_EXECUTABLE( dbsks_xio_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbsks_xio_test_include dbsks_xio )
