# This is basic/dbsol/algo/tests

ADD_EXECUTABLE ( dbsol_algo_test_all
   # Driver source and utilities
   test_driver.cxx

   # The tests
   test_interp_linear.cxx
   test_interp_eno.cxx
   test_geno.cxx
   test_geno_shock.cxx

   test_interp_eno_3d.cxx
   test_interp_linear_3d.cxx
)

TARGET_LINK_LIBRARIES( dbsol_algo_test_all dbsol_algo testlib )

ADD_TEST( dbsol_algo_test_interp_linear dbsol_algo_test_all test_interp_linear)
ADD_TEST( dbsol_algo_test_geno dbsol_algo_test_all test_geno)
ADD_TEST( dbsol_algo_test_geno_shock dbsol_algo_test_all test_geno_shock)

ADD_EXECUTABLE( dbsol_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbsol_algo_test_include dbsol_algo )
