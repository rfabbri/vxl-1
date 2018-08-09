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

add_test( NAME  boxm_test_binary_io          COMMAND $<TARGET_FILE:boxm_test_all> test_binary_io  )
add_test( NAME  boxm_test_block_iter         COMMAND $<TARGET_FILE:boxm_test_all> test_block_iter  )
add_test( NAME  boxm_block_vis_graph         COMMAND $<TARGET_FILE:boxm_test_all> test_block_vis_graph  ) 
add_test( NAME  boxm_test_cell_vis_graph     COMMAND $<TARGET_FILE:boxm_test_all> test_cell_vis_graph  )
add_test( NAME  boxm_test_quad_interpolate   COMMAND $<TARGET_FILE:boxm_test_all> test_quad_interpolate ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )
add_test( NAME  boxm_test_render_image       COMMAND $<TARGET_FILE:boxm_test_all> test_render_image  )
add_test( NAME  boxm_test_save_raw           COMMAND $<TARGET_FILE:boxm_test_all> test_save_raw  )
add_test( NAME  boxm_test_update             COMMAND $<TARGET_FILE:boxm_test_all> test_update  )
add_test( NAME  boxm_test_update_multi_bin   COMMAND $<TARGET_FILE:boxm_test_all> test_update_multi_bin  )
add_test( NAME  boxm_test_compute_visibility COMMAND $<TARGET_FILE:boxm_test_all> test_compute_visibility  )
add_test( NAME  boxm_test_boxm_rational_camera_utils COMMAND $<TARGET_FILE:boxm_test_all> test_boxm_rational_camera_utils ${CMAKE_CURRENT_SOURCE_DIR} )
add_test( NAME  boxm_test_upload_mesh COMMAND $<TARGET_FILE:boxm_test_all> test_upload_mesh  )
add_test( NAME  boxm_test_fill_in_mesh COMMAND $<TARGET_FILE:boxm_test_all> test_fill_in_mesh  )
add_test( NAME  boxm_test_save_scene_raw     COMMAND $<TARGET_FILE:boxm_test_all> test_save_scene_raw  )

add_executable( boxm_test_include test_include.cxx )
target_link_libraries( boxm_test_include boxm boct)
