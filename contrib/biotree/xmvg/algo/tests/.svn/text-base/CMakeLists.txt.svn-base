# contrib/biotree/xmvg/tests/CMakeLists.txt

ADD_EXECUTABLE( xmvg_algo_test_all   
  test_driver.cxx  
  test_ramp_compensation.cxx  
              )

TARGET_LINK_LIBRARIES(xmvg_algo_test_all xmvg xmvg_algo testlib )

ADD_TEST( xmvg_test_ramp_compensation   xmvg_test_all test_ramp_compensation)

