# /contrib/biotree/xscan/tests/CMakeLists.txt
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )

ADD_EXECUTABLE( xscan_test_all   
                test_driver.cxx  
                test_xscan.cxx 
                test_xscan_uniform_orbit.cxx
                test_xscan_scan.cxx
                test_xscan_dummy_scan.cxx
              )

TARGET_LINK_LIBRARIES(xscan_test_all xscan testlib)

ADD_TEST(xscan_test xscan_test_all)

