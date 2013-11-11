# brcv/seg/dbsta/tests/CMakeLists.txt
# Tests for dbsta library

ADD_EXECUTABLE( dbsta_test_all
  test_driver.cxx
  test_gaussian_full.cxx
  test_gaussian_indep.cxx
  test_gaussian_sphere.cxx
  test_mixture.cxx
  test_histogram.cxx
  test_dbsta_io.cxx
)

TARGET_LINK_LIBRARIES( dbsta_test_all dbsta testlib vpl)

ADD_TEST( dbsta_test_gaussian_full           dbsta_test_all test_gaussian_full )
ADD_TEST( dbsta_test_gaussian_indep          dbsta_test_all test_gaussian_indep )
ADD_TEST( dbsta_test_gaussian_sphere         dbsta_test_all test_gaussian_sphere )
ADD_TEST( dbsta_test_mixture                 dbsta_test_all test_mixture )
ADD_TEST( dbsta_test_histogram               dbsta_test_all test_histogram )
ADD_TEST( dbsta_test_io               dbsta_test_all test_dbsta_io )


ADD_EXECUTABLE( dbsta_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( dbsta_test_include dbsta )
