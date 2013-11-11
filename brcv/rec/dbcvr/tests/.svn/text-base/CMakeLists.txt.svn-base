# brcv/rec/dbcvr/tests/CMakeLists.txt

#INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )

ADD_EXECUTABLE( dbcvr_test_all
  test_driver.cxx
  test_cvmatch.cxx
  test_clsd_cvmatch.cxx
  test_clsd_interp_cvmatch.cxx
  test_interp_cvmatch.cxx
  test_cvmatch_even.cxx
)
TARGET_LINK_LIBRARIES( dbcvr_test_all dbcvr bsol testlib dbsol)

ADD_TEST( dbcvr_cvmatch              dbcvr_test_all test_cvmatch ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
ADD_TEST( dbcvr_cvmatch_even         dbcvr_test_all test_cvmatch_even)
ADD_TEST( dbcvr_clsd_cvmatch         dbcvr_test_all test_clsd_cvmatch ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
ADD_TEST( dbcvr_interp_cvmatch       dbcvr_test_all test_interp_cvmatch ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
ADD_TEST( dbcvr_clsd_interp_cvmatch  dbcvr_test_all test_clsd_interp_cvmatch ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)

ADD_EXECUTABLE( dbcvr_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbcvr_test_include dbcvr )
