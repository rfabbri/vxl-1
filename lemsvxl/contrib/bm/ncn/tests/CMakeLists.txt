# contrib/bm/ncn/tests/CMakeLists.txt
# Tests for ncn library

add_executable( ncn_test_all 
                test_driver.cxx
                test_mutual_information_ranking.cxx 
                test_factory.cxx )
  
target_link_libraries( ncn_test_all ncn vnl vul testlib )
 
add_test( NAME  ncn_test_mutual_information_ranking     COMMAND $<TARGET_FILE:ncn_test_all> test_mutual_information_ranking )
add_test( NAME  ncn_test_factory COMMAND $<TARGET_FILE:ncn_test_all> test_mutual_information_ranking )
 
