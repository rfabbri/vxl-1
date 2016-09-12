# /contrib/biotree/xscan/tests/CMakeLists.txt
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )

add_executable( xscan_test_all   
                test_driver.cxx  
                test_xscan.cxx 
                test_xscan_uniform_orbit.cxx
                test_xscan_scan.cxx
                test_xscan_dummy_scan.cxx
              )

target_link_libraries(xscan_test_all xscan testlib)

add_test(xscan_test xscan_test_all)

