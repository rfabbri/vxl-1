# dbrl/dbbas/dbsta/tests/CMakeLists.txt
# Tests for dbsta library

add_executable( dbstao_test_all
  test_driver.cxx
  test_gaussian_full.cxx
  test_gaussian_indep.cxx
  test_gaussian_sphere.cxx
  test_mixture.cxx
  test_bsta_histogram.cxx
  test_k_medoid.cxx
  test_otsu_threshold.cxx
  test_parzen_sphere.cxx
  test_weibull.cxx
  test_sampler.cxx
  test_von_mises.cxx
  test_beta.cxx
)

target_link_libraries( dbstao_test_all dbstao vnl vpl vbl_io vsl vbl testlib )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/beta_distr_2_2.txt ${CMAKE_CURRENT_BINARY_DIR}/beta_distr_2_2.txt COPYONLY )

add_test( NAME  dbsta_test_gaussian_full           COMMAND $<TARGET_FILE:bsta_test_all> test_gaussian_full )
add_test( NAME  dbsta_test_gaussian_indep          COMMAND $<TARGET_FILE:bsta_test_all> test_gaussian_indep )
add_test( NAME  dbsta_test_gaussian_sphere         COMMAND $<TARGET_FILE:bsta_test_all> test_gaussian_sphere )
add_test( NAME  dbsta_test_parzen_sphere           COMMAND $<TARGET_FILE:bsta_test_all> test_parzen_sphere )
add_test( NAME  dbsta_test_weibull                 COMMAND $<TARGET_FILE:bsta_test_all> test_weibull )
add_test( NAME  dbsta_test_von_mises               COMMAND $<TARGET_FILE:bsta_test_all> test_von_mises )
add_test( NAME  dbsta_test_mixture                 COMMAND $<TARGET_FILE:bsta_test_all> test_mixture )
add_test( NAME  dbsta_test_histogram               COMMAND $<TARGET_FILE:bsta_test_all> test_bsta_histogram )
add_test( NAME  dbsta_test_k_medoid                COMMAND $<TARGET_FILE:bsta_test_all> test_k_medoid )
add_test( NAME  dbsta_test_otsu_threshold          COMMAND $<TARGET_FILE:bsta_test_all> test_otsu_threshold )
add_test( NAME  dbsta_test_sampler                 COMMAND $<TARGET_FILE:bsta_test_all> test_sampler )
add_test( NAME  dbsta_test_beta                    COMMAND $<TARGET_FILE:bsta_test_all> test_beta )

add_executable( dbstao_test_include test_include.cxx )
target_link_libraries( dbstao_test_include dbstao vsl)
