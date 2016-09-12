# seg/dbsks/tests/CMakeLists.txt
# Tests for dbsks library

add_executable( dbsks_test_all
  test_driver.cxx
#  test_xio_xshock.cxx
)
target_link_libraries( dbsks_test_all dbsks testlib)

#add_test( dbsks_test_xio_xshock dbsks_test_all test_xio_xshock )

add_executable( dbsks_test_include test_include.cxx )
target_link_libraries( dbsks_test_include dbsks )
