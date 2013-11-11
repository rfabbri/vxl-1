# brcv/seg/dbbgm/algo/tests/CMakeLists.txt
# Tests for dbbgm_algo library

ADD_EXECUTABLE( dbbgm_algo_test_all
  test_driver.cxx
  test_hmm_algo.cxx
)

TARGET_LINK_LIBRARIES( dbbgm_algo_test_all dbbgm_algo testlib )

ADD_TEST( dbbgm_algo_test_hmm_algo           dbbgm_algo_test_all test_hmm_algo )

ADD_EXECUTABLE( dbbgm_algo_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( dbbgm_algo_test_include dbbgm_algo )
