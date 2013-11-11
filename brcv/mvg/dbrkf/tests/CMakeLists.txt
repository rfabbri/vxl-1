# algo/dbetl/tests/CMakeLists.txt


ADD_EXECUTABLE( dbrkf_test_all
  test_driver.cxx
  test_simplerect.cxx
)

TARGET_LINK_LIBRARIES( dbrkf_test_all dbrkf testlib )

ADD_TEST( dbrkf_simplerect  dbrkf_test_all test_simplerect ${CMAKE_CURRENT_SOURCE_DIR}/data )

ADD_EXECUTABLE( dbrkf_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbrkf_test_include dbrkf )
