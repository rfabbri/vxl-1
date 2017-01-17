# basic/dbgl/algo/tests/CMakeLists.txt
# Tests for dbgl_algo library

include_directories(${LIBRARY_OUTPUT_PATH})

add_executable( dbgl_algo_test_all
  test_driver.cxx
  test_diffgeom.cxx
)

target_link_libraries(dbgl_algo_test_all dbgl_algo bgld_algo testlib)

add_test( dbgl_algo_test_diffgeom dbgl_algo_test_all test_diffgeom )

add_executable( dbgl_algo_test_include test_include.cxx )
target_link_libraries( dbgl_algo_test_include dbgl_algo)
