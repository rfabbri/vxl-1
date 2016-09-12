# brcv/rec/dbcvr/tests/CMakeLists.txt

#include_directories(${GEL_INCLUDE_DIR} )

add_executable( dbcvr_test_all
  test_driver.cxx
  test_cvmatch.cxx
  test_clsd_cvmatch.cxx
  test_clsd_interp_cvmatch.cxx
  test_interp_cvmatch.cxx
  test_cvmatch_even.cxx
)
target_link_libraries( dbcvr_test_all dbcvr bsol testlib dbsol)

add_test( dbcvr_cvmatch              dbcvr_test_all test_cvmatch ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
add_test( dbcvr_cvmatch_even         dbcvr_test_all test_cvmatch_even)
add_test( dbcvr_clsd_cvmatch         dbcvr_test_all test_clsd_cvmatch ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
add_test( dbcvr_interp_cvmatch       dbcvr_test_all test_interp_cvmatch ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)
add_test( dbcvr_clsd_interp_cvmatch  dbcvr_test_all test_clsd_interp_cvmatch ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)

add_executable( dbcvr_test_include test_include.cxx )
target_link_libraries( dbcvr_test_include dbcvr )
