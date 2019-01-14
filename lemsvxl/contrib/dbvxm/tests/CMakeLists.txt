include_directories( ${VXL_BRL_SOURCE_DIR}/bseg/bvxm )
include_directories( ${LEMS_SOURCE_DIR}/contrib/dbvxm )

add_executable( dbvxm_test_all
  test_driver.cxx
  test_gen_synthetic_multi_scale_world.cxx
)

target_link_libraries( dbvxm_test_all dbvxm testlib vcl vbl vpgl bsta bsta_algo)


add_test( NAME  dbvxm_test_gen_synthetic_multi_scale_world     COMMAND $<TARGET_FILE:dbvxm_test_all> test_gen_synthetic_multi_scale_world )

add_executable( dbvxm_test_include test_include.cxx )
target_link_libraries( dbvxm_test_include dbvxm)
