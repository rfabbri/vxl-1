# contrib/bm/ncn/tests/CMakeLists.txt
# Tests for ncn library

add_executable( ncn_test_all 
                test_driver.cxx
                test_mutual_information_ranking.cxx 
                test_factory.cxx )
  
target_link_libraries( ncn_test_all ncn vnl vul testlib )
 
add_test( ncn_test_mutual_information_ranking     ${EXECUTABLE_OUTPUT_PATH}/ncn_test_all test_mutual_information_ranking )
add_test( ncn_test_factory ${EXECUTABLE_OUTPUT_PATH}/ncn_test_all test_mutual_information_ranking )
 
