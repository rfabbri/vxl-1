add_executable( boxm_test_all
  test_driver.cxx
  test_binary_io.cxx
  test_block_iter.cxx
  test_block_vis_graph.cxx
  test_cell_vis_graph.cxx

  test_utils.h test_utils.cxx
  test_quad_interpolate.cxx
  test_render_image.cxx
  test_save_raw.cxx
  test_update.cxx
  test_update_multi_bin.cxx
  test_compute_visibility.cxx
  test_boxm_rational_camera_utils.cxx
  test_upload_mesh.cxx
  test_fill_in_mesh.cxx
  test_save_scene_raw.cxx
 )

target_link_libraries( boxm_test_all boxm boct testlib vnl vgl_xio vpl vul vpgl)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/camera0.txt ${CMAKE_CURRENT_BINARY_DIR}/camera0.txt COPYONLY )

add_test( boxm_test_binary_io          ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_binary_io  )
add_test( boxm_test_block_iter         ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_block_iter  )
add_test( boxm_block_vis_graph         ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_block_vis_graph  ) 
add_test( boxm_test_cell_vis_graph     ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_cell_vis_graph  )
add_test( boxm_test_quad_interpolate   ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_quad_interpolate ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )
add_test( boxm_test_render_image       ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_render_image  )
add_test( boxm_test_save_raw           ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_save_raw  )
add_test( boxm_test_update             ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_update  )
add_test( boxm_test_update_multi_bin   ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_update_multi_bin  )
add_test( boxm_test_compute_visibility ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_compute_visibility  )
add_test( boxm_test_boxm_rational_camera_utils ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_boxm_rational_camera_utils ${CMAKE_CURRENT_SOURCE_DIR} )
add_test( boxm_test_upload_mesh ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_upload_mesh  )
add_test( boxm_test_fill_in_mesh ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_fill_in_mesh  )
add_test( boxm_test_save_scene_raw     ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_save_scene_raw  )

add_executable( boxm_test_include test_include.cxx )
target_link_libraries( boxm_test_include boxm boct)
