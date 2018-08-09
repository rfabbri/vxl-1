# contrib/bm/ncn1/test/CMakeLists.txt
# Tests for ncn1 library

add_executable( ncn1_test_all
                test_driver.cxx
                test_sampler.cxx
                test_factory.cxx )
                
target_link_libraries( ncn1_test_all ncn1 vnl vul testlib )

add_test( NAME  ncn1_test_sampler       COMMAND $<TARGET_FILE:ncn1_test_all> test_sampler )                
add_test( NAME  ncn1_test_factory       COMMAND $<TARGET_FILE:ncn1_test_all> test_factory )
