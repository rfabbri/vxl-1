# contrib/bm/ncn1/test/CMakeLists.txt
# Tests for ncn1 library

ADD_EXECUTABLE( ncn1_test_all
                test_driver.cxx
                test_sampler.cxx
                test_factory.cxx )
                
TARGET_LINK_LIBRARIES( ncn1_test_all ncn1 vnl vul testlib )

ADD_TEST( ncn1_test_sampler       ${EXECUTABLE_OUTPUT_PATH}/ncn1_test_all test_sampler )                
ADD_TEST( ncn1_test_factory       ${EXECUTABLE_OUTPUT_PATH}/ncn1_test_all test_factory )