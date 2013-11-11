# This is /seg/dbdet/algo/tests

ADD_EXECUTABLE ( dbdet_algo_test_all
   # Driver source and utilities
   test_driver.cxx

   # The tests
   test_load_edg.cxx
   
   test_sel.cxx
)

TARGET_LINK_LIBRARIES( dbdet_algo_test_all dbdet_algo testlib )
ADD_TEST( dbdet_algo_test_load_edg dbdet_algo_test_all test_load_edg )

ADD_TEST( dbdet_algo_test_sel   dbdet_algo_test_all test_sel )

ADD_EXECUTABLE( dbdet_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbdet_algo_test_include dbdet_algo )

