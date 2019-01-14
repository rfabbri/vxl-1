
include_directories( ${LEMS_SOURCE_DIR}/contrib/bvam )

add_executable( bvam_pro_test_all
  test_driver.cxx
  test_bvam_roi_init_process.cxx
  test_bvam_illum_index_process.cxx
  test_bvam_create_voxel_world_process.cxx
  test_bvam_normalize_image_process.cxx
  test_bvam_change_detection_display_process.cxx
  test_bvam_generate_edge_map_process.cxx
  test_bvam_rpc_registration_process.cxx
)

target_link_libraries( bvam_pro_test_all testlib bvam_pro bvam vcl vbl vpgl_algo vpgl vpgl_io vpgl_bpro bprb brdb vil vil_io vil_bpro)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/roi_params.xml ${CMAKE_CURRENT_BINARY_DIR}/roi_params.xml COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/world_model_params.xml ${CMAKE_CURRENT_BINARY_DIR}/world_model_params.xml COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/world_model_params_lvcs.txt ${CMAKE_CURRENT_BINARY_DIR}/world_model_params_lvcs.txt COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/change_display_params.xml ${CMAKE_CURRENT_BINARY_DIR}/change_display_params.xml COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/edge_map_params.xml ${CMAKE_CURRENT_BINARY_DIR}/edge_map_params.xml COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/two_box_synth.tif ${CMAKE_CURRENT_BINARY_DIR}/two_box_synth.tif COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/po_39928_pan_0000000_chip_700_700_noICHIPB.nitf ${CMAKE_CURRENT_BINARY_DIR}/po_39928_pan_0000000_chip_700_700_noICHIPB.nitf COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/rpc_registration_image.png ${CMAKE_CURRENT_BINARY_DIR}/rpc_registration_image.png COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/rpc_registration_camera.rpb ${CMAKE_CURRENT_BINARY_DIR}/rpc_registration_camera.rpb COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/rpc_registration_parameters.xml ${CMAKE_CURRENT_BINARY_DIR}/rpc_registration_parameters.xml )

add_test( NAME  bvam_change_detection_display_process    COMMAND $<TARGET_FILE:bvam_pro_test_all>   test_bvam_change_detection_display_process )
add_test( NAME  bvam_test_roi_init_process       COMMAND $<TARGET_FILE:bvam_pro_test_all>   test_bvam_roi_init_process )
add_test( NAME  bvam_test_illum_index_process    COMMAND $<TARGET_FILE:bvam_pro_test_all>   test_bvam_illum_index_process )
add_test( NAME  bvam_test_create_voxel_world_process    COMMAND $<TARGET_FILE:bvam_pro_test_all>   test_bvam_create_voxel_world_process )
add_test( NAME  bvam_test_bvam_normalize_image_process    COMMAND $<TARGET_FILE:bvam_pro_test_all>   test_bvam_bvam_normalize_image_process )
add_test( NAME  bvam_test_generate_edge_map_process    COMMAND $<TARGET_FILE:bvam_pro_test_all>   test_bvam_generate_edge_map_process )
add_test( NAME  bvam_test_rpc_registration_process    COMMAND $<TARGET_FILE:bvam_pro_test_all>   test_bvam_rpc_registration_process )
