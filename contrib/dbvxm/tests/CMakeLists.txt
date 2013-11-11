INCLUDE_DIRECTORIES( ${VXL_BRL_SOURCE_DIR}/bseg/bvxm )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbvxm )

ADD_EXECUTABLE( dbvxm_test_all
  test_driver.cxx
  test_gen_synthetic_multi_scale_world.cxx
)

TARGET_LINK_LIBRARIES( dbvxm_test_all dbvxm testlib vcl vbl vpgl bsta bsta_algo)


ADD_TEST( dbvxm_test_gen_synthetic_multi_scale_world     ${EXECUTABLE_OUTPUT_PATH}/dbvxm_test_all test_gen_synthetic_multi_scale_world )

ADD_EXECUTABLE( dbvxm_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbvxm_test_include dbvxm)
