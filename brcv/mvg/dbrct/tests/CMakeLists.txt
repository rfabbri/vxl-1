# brcv/dbrct/tests/CMakeLists.txt


add_executable( dbrct_test_all
  test_driver.cxx
  test_dense_match.cxx
)

target_link_libraries( dbrct_test_all dbrct testlib )
add_test(dbrct_test_dense_match dbrct_test_all test_dense_match ${CMAKE_CURRENT_SOURCE_DIR}/images ) 


add_executable( dbrct_test_include test_include.cxx )
target_link_libraries( dbrct_test_include dbrct )


