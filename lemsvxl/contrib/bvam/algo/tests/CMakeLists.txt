add_executable( bvam_algo_test_all
  test_driver.cxx
  test_ekf_camera_optimizer.cxx
)

target_link_libraries( bvam_algo_test_all bvam_algo bvam testlib vcl vbl vgl vpgl bsta bsta_algo)


add_test( bvam_test_ekf_camera_optimizer   ${EXECUTABLE_OUTPUT_PATH}/bvam_algo_test_all   test_ekf_camera_optimizer)




