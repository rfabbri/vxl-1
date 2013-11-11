# contrib/bm/vlfeat/tests/CMakeLists.txt
# Tests for vlfeat c++ bindings

ADD_EXECUTABLE( vlfeat_test_all
                test_driver.cxx
                vlfeat_test_helloWorld.cxx
                vlfeat_test_dsift.cxx )
                
TARGET_LINK_LIBRARIES( vlfeat_test_all vlfeat vul vnl testlib )         

ADD_TEST( vlfeat_test_helloWorld          ${EXECUTABLE_OUTPUT_PATH}/vlfeat_test_all test_helloWorld )
ADD_TEST( vlfeat_test_dsift               ${EXECUTABLE_OUTPUT_PATH}/vlfeat_test_all test_dsift      )     