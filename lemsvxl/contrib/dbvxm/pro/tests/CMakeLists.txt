include_directories( ${VXL_BRL_SOURCE_DIR} )
include_directories( ${VXL_BRL_SOURCE_DIR}/bseg/bvxm )
include_directories( ${VXL_BRL_SOURCE_DIR}/bpro/core )

include_directories( ${LEMS_SOURCE_DIR}/contrib/dbvxm )

add_executable( dbvxm_pro_test_all
  test_driver.cxx
  test_dbvxm_detect_instance_process.cxx
  test_dbvxm_detect_scale_process.cxx
)

target_link_libraries( dbvxm_pro_test_all testlib dbvxm_pro bvxm_pro bvxm vcl vbl vpgl_algo vpgl vpgl_io vpgl_pro bprb brdb vil vil_io vil_pro)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/roi_params.xml ${CMAKE_CURRENT_BINARY_DIR}/roi_params.xml COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/world_model_params.xml ${CMAKE_CURRENT_BINARY_DIR}/world_model_params.xml COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/world_model_params_lvcs.txt ${CMAKE_CURRENT_BINARY_DIR}/world_model_params_lvcs.txt COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/po_39928_pan_0000000_chip_700_700_noICHIPB.nitf ${CMAKE_CURRENT_BINARY_DIR}/po_39928_pan_0000000_chip_700_700_noICHIPB.nitf COPYONLY )
add_test( NAME  dbvxm_test_dbvxm_detect_instance_process    COMMAND $<TARGET_FILE:dbvxm_pro_test_all>   test_dbvxm_detect_instance_process )

add_executable( dbvxm_pro_test_include test_include.cxx )
target_link_libraries( dbvxm_pro_test_include dbvxm_pro bvxm_pro )
