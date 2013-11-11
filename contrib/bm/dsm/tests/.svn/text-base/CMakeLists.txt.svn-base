# contrib/bm/dsm/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )

ADD_EXECUTABLE( dsm_test_all
                test_driver.cxx
                dsm_test_map_binary_io.cxx
                dsm_test_frame_clock_binary_io.cxx
                dsm_test_node_binary_io.cxx
                dsm_test_state_machine_binary_io.cxx
                dsm_test_manager.cxx
 )
                
TARGET_LINK_LIBRARIES( dsm_test_all dsm dsm_io vul vnl vbl testlib )

ADD_TEST( dsm_test_node_binary_io                       ${EXECUTABLE_OUTPUT_PATH}/dsm_test_all test_node_binary_io )
ADD_TEST( dsm_test_state_machine_binary_io              ${EXECUTABLE_OUTPUT_PATH}/dsm_test_all test_state_machine_binary_io )
ADD_TEST( dsm_test_frame_clock_binary_io                ${EXECUTABLE_OUTPUT_PATH}/dsm_test_all test_frame_clock_binary_io )
ADD_TEST( dsm_test_mapbinary_io                         ${EXECUTABLE_OUTPUT_PATH}/dsm_test_all test_map_binary_io )
ADD_TEST( dsm_test_manager                              ${EXECUTABLE_OUTPUT_PATH}/dsm_test_all test_manager )
