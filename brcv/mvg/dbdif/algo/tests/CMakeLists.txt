# This is brcv/mvg/dbdif/algo/tests/CMakeLists.txt

ADD_EXECUTABLE (dbdif_algo_test_all
   # Driver source and utilities
   test_driver.cxx

   # The tests
   test_diff_geometry.cxx
)

TARGET_LINK_LIBRARIES( dbdif_algo_test_all dbdif_algo testlib )

ADD_TEST( dbdif_algo_test_diff_geometry dbdif_algo_test_all test_diff_geometry )

ADD_EXECUTABLE( dbdif_algo_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( dbdif_algo_test_include dbdif_algo )
