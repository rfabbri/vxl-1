# contrib/bm/ncn/tests/CMakeLists.txt
# Tests for ncn library

ADD_EXECUTABLE( ncn_test_all 
                test_driver.cxx
                test_mutual_information_ranking.cxx 
                test_factory.cxx )
  
TARGET_LINK_LIBRARIES( ncn_test_all ncn vnl vul testlib )
 
ADD_TEST( ncn_test_mutual_information_ranking     ${EXECUTABLE_OUTPUT_PATH}/ncn_test_all test_mutual_information_ranking )
ADD_TEST( ncn_test_factory ${EXECUTABLE_OUTPUT_PATH}/ncn_test_all test_mutual_information_ranking )
 
