# This is /seg/dbdet/algo/tests

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/basic/dbtest )

ADD_EXECUTABLE ( dbdet_algo_test_all
   # Driver source and utilities
   test_driver.cxx

   # The tests
   test_load_edg.cxx
   
   test_sel.cxx

   test_edgemap.cxx

   test_curve_fragment_postprocess.cxx
)

TARGET_LINK_LIBRARIES( dbdet_algo_test_all dbdet_algo sdet testlib dbtest)

ADD_TEST( dbdet_algo_test_load_edg dbdet_algo_test_all test_load_edg )

ADD_TEST( dbdet_algo_test_sel   dbdet_algo_test_all test_sel )

ADD_TEST ( dbdet_algo_test_edgemap dbdet_algo_test_all test_edgemap )

ADD_TEST ( dbdet_algo_test_curve_fragment_postprocess dbdet_algo_test_all test_curve_fragment_postprocess )

ADD_EXECUTABLE( dbdet_algo_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( dbdet_algo_test_include dbdet_algo )

