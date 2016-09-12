# brcv/seg/dbbgm/algo/tests/CMakeLists.txt
# Tests for dbbgm_algo library

add_executable( dbbgm_algo_test_all
  test_driver.cxx
  test_hmm_algo.cxx
)

target_link_libraries( dbbgm_algo_test_all dbbgm_algo testlib )

add_test( dbbgm_algo_test_hmm_algo           dbbgm_algo_test_all test_hmm_algo )

add_executable( dbbgm_algo_test_include test_include.cxx )

target_link_libraries( dbbgm_algo_test_include dbbgm_algo )
