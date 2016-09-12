add_executable( dbccl_test_all
  test_driver.cxx
  test_aerial_tracker.cxx
  test_camera_estimator.cxx
  test_manual_camera.cxx
  test_fast_mi.cxx
)

target_link_libraries( dbccl_test_all dbccl vgl vnl vil vidl1 vpl testlib )
add_test( dbccl_test_tracker           dbccl_test_all   test_aerial_tracker )
add_test( dbccl_test_camera_estimator  dbccl_test_all   test_camera_estimator )
add_test( dbccl_test_manual_camera     dbccl_test_all   test_manual_camera )
add_test( dbccl_test_fast_mi           dbccl_test_all   test_fast_mi )
