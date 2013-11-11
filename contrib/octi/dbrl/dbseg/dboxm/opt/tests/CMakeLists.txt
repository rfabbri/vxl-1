ADD_EXECUTABLE( dboxm_opt_test_all
  test_driver.cxx
  test_bayesian_optimizer.cxx
 )

TARGET_LINK_LIBRARIES( dboxm_opt_test_all dboxm_opt dboxm boct testlib vnl vgl_xio vpl vul vpgl)

ADD_TEST( dboxm_test_bayesian_optimizer     ${EXECUTABLE_OUTPUT_PATH}/dboxm_opt_test_all test_bayesian_optimizer  )

ADD_EXECUTABLE( dboxm_opt_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dboxm_opt_test_include dboxm_opt dboxm boct)
