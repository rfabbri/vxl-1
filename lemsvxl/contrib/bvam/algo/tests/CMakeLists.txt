add_executable( bvam_algo_test_all
  test_driver.cxx
  test_ekf_camera_optimizer.cxx
)

target_link_libraries( bvam_algo_test_all bvam_algo bvam testlib vcl vbl vgl vpgl bsta bsta_algo)


add_test( NAME  bvam_test_ekf_camera_optimizer   COMMAND $<TARGET_FILE:bvam_algo_test_all>   test_ekf_camera_optimizer)




