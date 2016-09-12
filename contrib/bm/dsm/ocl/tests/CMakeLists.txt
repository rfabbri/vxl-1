#contrib/bm/dsm/ocl/tests/CMakeLists.txt
#Tests for dsm_ocl library

add_executable( dsm_ocl_test_all 
                test_driver.cxx
                test_hog.cxx )
                
target_link_libraries( dsm_ocl_test_all dsm_ocl vnl vul )

add_test( dsm_ocl_test_hog      ${EXECUTABLE_OUTPUT_PATH}/dsm_test_all test_hog )               