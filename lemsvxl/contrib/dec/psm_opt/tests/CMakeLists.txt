include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )

add_executable( psm_opt_test_all
  test_driver.cxx
  gen_synthetic_scene.h  gen_synthetic_scene.cxx
  test_psm_opt_bayesian_optimizer.cxx
  test_psm_opt_optimizer.cxx
  test_psm_opt_generate_opt_samples.cxx
  test_psm_opt_appearance_estimator.cxx
)

target_link_libraries( psm_opt_test_all psm psm_algo psm_opt hsds testlib vbl vgl bvxm vnl vpgl)


add_test( psm_opt_generate_opt_samples ${EXECUTABLE_OUTPUT_PATH}/psm_opt_test_all   test_psm_opt_generate_opt_samples)
add_test( psm_opt_optimizer ${EXECUTABLE_OUTPUT_PATH}/psm_opt_test_all   test_psm_opt_optimizer)
add_test( psm_opt_bayesian_optimizer ${EXECUTABLE_OUTPUT_PATH}/psm_opt_test_all   test_psm_opt_bayesian_optimizer)
add_test( psm_opt_appearance_estimator ${EXECUTABLE_OUTPUT_PATH}/psm_opt_test_all test_psm_opt_appearance_estimator)





