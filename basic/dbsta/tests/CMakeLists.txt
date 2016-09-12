# brcv/seg/dbsta/tests/CMakeLists.txt
# Tests for dbsta library

add_executable( dbsta_test_all
  test_driver.cxx
  test_gaussian_full.cxx
  test_gaussian_indep.cxx
  test_gaussian_sphere.cxx
  test_mixture.cxx
  test_histogram.cxx
  test_dbsta_io.cxx
)

target_link_libraries( dbsta_test_all dbsta testlib vpl)

add_test( dbsta_test_gaussian_full           dbsta_test_all test_gaussian_full )
add_test( dbsta_test_gaussian_indep          dbsta_test_all test_gaussian_indep )
add_test( dbsta_test_gaussian_sphere         dbsta_test_all test_gaussian_sphere )
add_test( dbsta_test_mixture                 dbsta_test_all test_mixture )
add_test( dbsta_test_histogram               dbsta_test_all test_histogram )
add_test( dbsta_test_io               dbsta_test_all test_dbsta_io )


add_executable( dbsta_test_include test_include.cxx )

target_link_libraries( dbsta_test_include dbsta )
