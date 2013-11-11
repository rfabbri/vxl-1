# brcv/dbrct/tests/CMakeLists.txt


ADD_EXECUTABLE( dbrct_test_all
  test_driver.cxx
  test_dense_match.cxx
)

TARGET_LINK_LIBRARIES( dbrct_test_all dbrct testlib )
ADD_TEST(dbrct_test_dense_match dbrct_test_all test_dense_match ${CMAKE_CURRENT_SOURCE_DIR}/images ) 


ADD_EXECUTABLE( dbrct_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbrct_test_include dbrct )


