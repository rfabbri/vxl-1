#contrib/bm/dsm/ocl/tests/CMakeLists.txt
#Tests for dsm_ocl library

ADD_EXECUTABLE( dsm_ocl_test_all 
                test_driver.cxx
                test_hog.cxx )
                
TARGET_LINK_LIBRARIES( dsm_ocl_test_all dsm_ocl vnl vul )

ADD_TEST( dsm_ocl_test_hog      ${EXECUTABLE_OUTPUT_PATH}/dsm_test_all test_hog )               