INCLUDE_DIRECTORIES( ${VXL_BRL_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_BRL_SOURCE_DIR}/bseg/bvxm )
INCLUDE_DIRECTORIES( ${VXL_BRL_SOURCE_DIR}/bpro/core )

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dbvxm )

ADD_EXECUTABLE( dbvxm_pro_test_all
  test_driver.cxx
  test_dbvxm_detect_instance_process.cxx
  test_dbvxm_detect_scale_process.cxx
)

TARGET_LINK_LIBRARIES( dbvxm_pro_test_all testlib dbvxm_pro bvxm_pro bvxm vcl vbl vpgl_algo vpgl vpgl_io vpgl_pro bprb brdb vil vil_io vil_pro)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/roi_params.xml ${CMAKE_CURRENT_BINARY_DIR}/roi_params.xml COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/world_model_params.xml ${CMAKE_CURRENT_BINARY_DIR}/world_model_params.xml COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/world_model_params_lvcs.txt ${CMAKE_CURRENT_BINARY_DIR}/world_model_params_lvcs.txt COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/po_39928_pan_0000000_chip_700_700_noICHIPB.nitf ${CMAKE_CURRENT_BINARY_DIR}/po_39928_pan_0000000_chip_700_700_noICHIPB.nitf COPYONLY )
ADD_TEST( dbvxm_test_dbvxm_detect_instance_process    ${EXECUTABLE_OUTPUT_PATH}/dbvxm_pro_test_all   test_dbvxm_detect_instance_process )

ADD_EXECUTABLE( dbvxm_pro_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbvxm_pro_test_include dbvxm_pro bvxm_pro )
