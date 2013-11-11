ADD_EXECUTABLE( bvam_algo_test_all
  test_driver.cxx
  test_ekf_camera_optimizer.cxx
)

TARGET_LINK_LIBRARIES( bvam_algo_test_all bvam_algo bvam testlib vcl vbl vgl vpgl bsta bsta_algo)


ADD_TEST( bvam_test_ekf_camera_optimizer   ${EXECUTABLE_OUTPUT_PATH}/bvam_algo_test_all   test_ekf_camera_optimizer)




