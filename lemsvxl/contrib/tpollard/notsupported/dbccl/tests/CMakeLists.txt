add_executable( dbccl_test_all
  test_driver.cxx
  test_aerial_tracker.cxx
  test_camera_estimator.cxx
  test_manual_camera.cxx
  test_fast_mi.cxx
)

target_link_libraries( dbccl_test_all dbccl vgl vnl vil vidl1 vpl testlib )
add_test( NAME  dbccl_test_tracker           COMMAND $<TARGET_FILE:dbccl_test_all>   test_aerial_tracker )
add_test( NAME  dbccl_test_camera_estimator  COMMAND $<TARGET_FILE:dbccl_test_all>   test_camera_estimator )
add_test( NAME  dbccl_test_manual_camera     COMMAND $<TARGET_FILE:dbccl_test_all>   test_manual_camera )
add_test( NAME  dbccl_test_fast_mi           COMMAND $<TARGET_FILE:dbccl_test_all>   test_fast_mi )
