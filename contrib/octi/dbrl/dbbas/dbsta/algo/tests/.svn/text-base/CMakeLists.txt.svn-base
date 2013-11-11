# dbrl/dbbas/dbsta/algo/tests/CMakeLists.txt
# Tests for bsta/algo library

ADD_EXECUTABLE( dbstao_algo_test_all
  test_driver.cxx
  test_fit_weibull.cxx
  test_gaussian_model.cxx
  test_mean_shift.cxx
  test_von_mises_update.cxx
  test_beta_updater.cxx
)

TARGET_LINK_LIBRARIES( dbstao_algo_test_all dbstao_algo vnl vgl testlib )

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/beta_distr_100_100.txt ${CMAKE_CURRENT_BINARY_DIR}/beta_distr_100_100.txt COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/beta_distr_10_100.txt ${CMAKE_CURRENT_BINARY_DIR}/beta_distr_10_100.txt COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/beta_distr_100_10.txt ${CMAKE_CURRENT_BINARY_DIR}/beta_distr_100_10.txt COPYONLY )

ADD_TEST( dbstao_algo_test_fit_weibull  ${EXECUTABLE_OUTPUT_PATH}/bsta_algo_test_all test_fit_weibull )
ADD_TEST( dbstao_algo_test_gaussian_model ${EXECUTABLE_OUTPUT_PATH}/bsta_algo_test_all test_gaussian_model )
ADD_TEST( dbstao_algo_test_mean_shift  ${EXECUTABLE_OUTPUT_PATH}/bsta_algo_test_all test_mean_shift )
ADD_TEST( dbstao_algo_test_von_mises_update  ${EXECUTABLE_OUTPUT_PATH}/bsta_algo_test_all test_von_mises_update )
ADD_TEST( dbstao_algo_test_beta_updater  ${EXECUTABLE_OUTPUT_PATH}/bsta_algo_test_all test_beta_updater )

ADD_EXECUTABLE( dbstao_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbstao_algo_test_include dbstao_algo )
