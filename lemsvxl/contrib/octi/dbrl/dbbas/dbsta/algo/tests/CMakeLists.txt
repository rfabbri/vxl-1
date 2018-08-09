# dbrl/dbbas/dbsta/algo/tests/CMakeLists.txt
# Tests for bsta/algo library

add_executable( dbstao_algo_test_all
  test_driver.cxx
  test_fit_weibull.cxx
  test_gaussian_model.cxx
  test_mean_shift.cxx
  test_von_mises_update.cxx
  test_beta_updater.cxx
)

target_link_libraries( dbstao_algo_test_all dbstao_algo vnl vgl testlib )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/beta_distr_100_100.txt ${CMAKE_CURRENT_BINARY_DIR}/beta_distr_100_100.txt COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/beta_distr_10_100.txt ${CMAKE_CURRENT_BINARY_DIR}/beta_distr_10_100.txt COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/beta_distr_100_10.txt ${CMAKE_CURRENT_BINARY_DIR}/beta_distr_100_10.txt COPYONLY )

add_test( NAME  dbstao_algo_test_fit_weibull  COMMAND $<TARGET_FILE:bsta_algo_test_all> test_fit_weibull )
add_test( NAME  dbstao_algo_test_gaussian_model COMMAND $<TARGET_FILE:bsta_algo_test_all> test_gaussian_model )
add_test( NAME  dbstao_algo_test_mean_shift  COMMAND $<TARGET_FILE:bsta_algo_test_all> test_mean_shift )
add_test( NAME  dbstao_algo_test_von_mises_update  COMMAND $<TARGET_FILE:bsta_algo_test_all> test_von_mises_update )
add_test( NAME  dbstao_algo_test_beta_updater  COMMAND $<TARGET_FILE:bsta_algo_test_all> test_beta_updater )

add_executable( dbstao_algo_test_include test_include.cxx )
target_link_libraries( dbstao_algo_test_include dbstao_algo )
