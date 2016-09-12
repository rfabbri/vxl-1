# contrib/bm/dsm/tests/CMakeLists.txt

include_directories( ${VXL_CORE_INCLUDE_DIR} )

add_executable( dsm_test_all
                test_driver.cxx
                dsm_test_map_binary_io.cxx
                dsm_test_frame_clock_binary_io.cxx
                dsm_test_node_binary_io.cxx
                dsm_test_state_machine_binary_io.cxx
                dsm_test_manager.cxx
 )
                
target_link_libraries( dsm_test_all dsm dsm_io vul vnl vbl testlib )

add_test( dsm_test_node_binary_io                       ${EXECUTABLE_OUTPUT_PATH}/dsm_test_all test_node_binary_io )
add_test( dsm_test_state_machine_binary_io              ${EXECUTABLE_OUTPUT_PATH}/dsm_test_all test_state_machine_binary_io )
add_test( dsm_test_frame_clock_binary_io                ${EXECUTABLE_OUTPUT_PATH}/dsm_test_all test_frame_clock_binary_io )
add_test( dsm_test_mapbinary_io                         ${EXECUTABLE_OUTPUT_PATH}/dsm_test_all test_map_binary_io )
add_test( dsm_test_manager                              ${EXECUTABLE_OUTPUT_PATH}/dsm_test_all test_manager )
