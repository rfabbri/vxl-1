# seg/dbsks/tests/CMakeLists.txt
# Tests for dbsks library

add_executable( dbsks_xio_test_all
  test_driver.cxx
  test_xio_xshock.cxx
)
target_link_libraries( dbsks_xio_test_all dbsks_xio testlib)

#add_test( NAME  dbsks_test_xio_xshock COMMAND $<TARGET_FILE:dbsks_test_all> test_xio_xshock )

# Test include files
add_executable( dbsks_xio_test_include test_include.cxx )
target_link_libraries( dbsks_xio_test_include dbsks_xio )
