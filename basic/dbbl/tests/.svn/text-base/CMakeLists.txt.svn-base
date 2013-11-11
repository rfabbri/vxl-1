# basic/dbbl/tests/CMakeLists.txt
# Tests for dbbl library

INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR} )

ADD_EXECUTABLE( dbbl_test_all
  test_driver.cxx
  test_subsequence.cxx
)

#TARGET_LINK_LIBRARIES( dbbl_test_all dbbl vbl testlib  )
TARGET_LINK_LIBRARIES( dbbl_test_all vsol vbl testlib  )

ADD_TEST( dbbl_test_subsequence  dbbl_test_all test_subsequence )

ADD_EXECUTABLE( dbbl_test_include test_include.cxx )

#TARGET_LINK_LIBRARIES( dbbl_test_include dbbl )
TARGET_LINK_LIBRARIES( dbbl_test_include vsol vbl )
