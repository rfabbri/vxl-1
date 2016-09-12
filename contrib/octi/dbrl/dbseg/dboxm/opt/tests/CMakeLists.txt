add_executable( dboxm_opt_test_all
  test_driver.cxx
  test_bayesian_optimizer.cxx
 )

target_link_libraries( dboxm_opt_test_all dboxm_opt dboxm boct testlib vnl vgl_xio vpl vul vpgl)

add_test( dboxm_test_bayesian_optimizer     ${EXECUTABLE_OUTPUT_PATH}/dboxm_opt_test_all test_bayesian_optimizer  )

add_executable( dboxm_opt_test_include test_include.cxx )
target_link_libraries( dboxm_opt_test_include dboxm_opt dboxm boct)
