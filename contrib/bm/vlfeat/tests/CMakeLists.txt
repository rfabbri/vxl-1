# contrib/bm/vlfeat/tests/CMakeLists.txt
# Tests for vlfeat c++ bindings

add_executable( vlfeat_test_all
                test_driver.cxx
                vlfeat_test_helloWorld.cxx
                vlfeat_test_dsift.cxx )
                
target_link_libraries( vlfeat_test_all vlfeat vul vnl testlib )         

add_test( vlfeat_test_helloWorld          ${EXECUTABLE_OUTPUT_PATH}/vlfeat_test_all test_helloWorld )
add_test( vlfeat_test_dsift               ${EXECUTABLE_OUTPUT_PATH}/vlfeat_test_all test_dsift      )     