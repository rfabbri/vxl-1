ADD_EXECUTABLE( bseg3d_algo_test_all
  test_driver.cxx
  test_gmm_l2norm.cxx
  test_merge_mixtures.cxx
  test_gauss_multiplier.cxx
  test_windows.cxx
)
TARGET_LINK_LIBRARIES( bseg3d_algo_test_all bseg3d_algo testlib vcl vnl bsta)

ADD_TEST( test_gmm_l2norm         ${EXECUTABLE_OUTPUT_PATH}/bseg3d_algo_test_all   bseg3d_algo_test_gmm_l2norm )

ADD_EXECUTABLE( bseg3d_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bseg3d_algo_test_include bseg3d_algo )
