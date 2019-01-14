include_directories( ${VXL_BRL_SOURCE_DIR}/bseg/bvxm )
include_directories( ${LEMS_SOURCE_DIR}/contrib/dbvxm )

add_executable( dbvxm_rec_test_all
  test_driver.cxx
  test_dbvxm_densities.cxx
  test_param_estimation.cxx
  test_dbvxm_parts.cxx
  test_dbvxm_hierarchy_builder.cxx
  test_stat_collector.cxx
)

target_link_libraries( dbvxm_rec_test_all dbvxm_rec testlib vcl vbl vpgl)


add_test( NAME  dbvxm_test_densities     COMMAND $<TARGET_FILE:dbvxm_test_all> test_dbvxm_densities )
add_test( NAME  dbvxm_test_param_estimation     COMMAND $<TARGET_FILE:dbvxm_test_all> test_dbvxm_param_estimation )
add_test( NAME  dbvxm_test_parts     COMMAND $<TARGET_FILE:dbvxm_test_all> test_dbvxm_parts )
add_test( NAME  dbvxm_test_hierarchy_builder     COMMAND $<TARGET_FILE:dbvxm_test_all> test_dbvxm_hierarchy_builder )
add_test( NAME  dbvxm_test_stat_collector     COMMAND $<TARGET_FILE:dbvxm_test_all> test_dbvxm_stat_collector )

add_executable( dbvxm_rec_test_include test_include.cxx )
target_link_libraries( dbvxm_rec_test_include dbvxm_rec)
