INCLUDE_DIRECTORIES( ${VXL_BRL_SOURCE_DIR}/bseg/bvxm )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbvxm )

ADD_EXECUTABLE( dbvxm_rec_test_all
  test_driver.cxx
  test_dbvxm_densities.cxx
  test_param_estimation.cxx
  test_dbvxm_parts.cxx
  test_dbvxm_hierarchy_builder.cxx
  test_stat_collector.cxx
)

TARGET_LINK_LIBRARIES( dbvxm_rec_test_all dbvxm_rec testlib vcl vbl vpgl)


ADD_TEST( dbvxm_test_densities     ${EXECUTABLE_OUTPUT_PATH}/dbvxm_test_all test_dbvxm_densities )
ADD_TEST( dbvxm_test_param_estimation     ${EXECUTABLE_OUTPUT_PATH}/dbvxm_test_all test_dbvxm_param_estimation )
ADD_TEST( dbvxm_test_parts     ${EXECUTABLE_OUTPUT_PATH}/dbvxm_test_all test_dbvxm_parts )
ADD_TEST( dbvxm_test_hierarchy_builder     ${EXECUTABLE_OUTPUT_PATH}/dbvxm_test_all test_dbvxm_hierarchy_builder )
ADD_TEST( dbvxm_test_stat_collector     ${EXECUTABLE_OUTPUT_PATH}/dbvxm_test_all test_dbvxm_stat_collector )

ADD_EXECUTABLE( dbvxm_rec_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbvxm_rec_test_include dbvxm_rec)
