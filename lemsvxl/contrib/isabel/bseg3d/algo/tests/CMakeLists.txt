add_executable( bseg3d_algo_test_all
  test_driver.cxx
  test_gmm_l2norm.cxx
  test_merge_mixtures.cxx
  test_gauss_multiplier.cxx
  test_windows.cxx
)
target_link_libraries( bseg3d_algo_test_all bseg3d_algo testlib vcl vnl bsta)

add_test( NAME  test_gmm_l2norm         COMMAND $<TARGET_FILE:bseg3d_algo_test_all>   bseg3d_algo_test_gmm_l2norm )

add_executable( bseg3d_algo_test_include test_include.cxx )
target_link_libraries( bseg3d_algo_test_include bseg3d_algo )
