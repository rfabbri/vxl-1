# basic/dbgl/tests/CMakeLists.txt


add_executable( dbgl_test_all
  test_driver.cxx
  test_minimal_container.cxx
)

target_link_libraries( dbgl_test_all dbgl testlib )

add_test( dbgl_test_minimal_container  dbgl_test_all test_minimal_container )

add_executable( dbgl_test_include test_include.cxx )
target_link_libraries( dbgl_test_include dbgl vsl )
