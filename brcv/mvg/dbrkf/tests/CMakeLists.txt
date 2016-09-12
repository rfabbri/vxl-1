# algo/dbetl/tests/CMakeLists.txt


add_executable( dbrkf_test_all
  test_driver.cxx
  test_simplerect.cxx
)

target_link_libraries( dbrkf_test_all dbrkf testlib )

add_test( dbrkf_simplerect  dbrkf_test_all test_simplerect ${CMAKE_CURRENT_SOURCE_DIR}/data )

add_executable( dbrkf_test_include test_include.cxx )
target_link_libraries( dbrkf_test_include dbrkf )
