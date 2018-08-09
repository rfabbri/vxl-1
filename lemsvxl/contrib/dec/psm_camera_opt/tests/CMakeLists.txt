add_executable( breg3d_test_all
  test_driver.cxx
  test_ekf_camera_optimizer.cxx
  test_ekf_camera_optimizer_seq.cxx
  test_homography_generator.cxx
)

target_link_libraries( breg3d_test_all breg3d bvxm testlib vcl vbl vgl vpgl bsta bsta_algo)


add_test( NAME  breg3d_test_ekf_camera_optimizer     COMMAND $<TARGET_FILE:breg3d_test_all>   test_ekf_camera_optimizer)
add_test( NAME  breg3d_test_ekf_camera_optimizer_seq COMMAND $<TARGET_FILE:breg3d_test_all>   test ekf_camera_optimizer_seq)
add_test( NAME  breg3d_test_homography_generator     COMMAND $<TARGET_FILE:breg3d_test_all>   test_homography_generator)





