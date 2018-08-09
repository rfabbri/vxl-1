# contrib/biotree/xmvg/tests/CMakeLists.txt

add_executable( xmvg_algo_test_all   
  test_driver.cxx  
  test_ramp_compensation.cxx  
              )

target_link_libraries(xmvg_algo_test_all xmvg xmvg_algo testlib )

add_test( NAME  xmvg_test_ramp_compensation   COMMAND $<TARGET_FILE:xmvg_test_all> test_ramp_compensation)

