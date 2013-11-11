ADD_EXECUTABLE( dbccl_test_all
  test_driver.cxx
  test_aerial_tracker.cxx
  test_camera_estimator.cxx
  test_manual_camera.cxx
  test_fast_mi.cxx
)

TARGET_LINK_LIBRARIES( dbccl_test_all dbccl vgl vnl vil vidl1 vpl testlib )
ADD_TEST( dbccl_test_tracker           dbccl_test_all   test_aerial_tracker )
ADD_TEST( dbccl_test_camera_estimator  dbccl_test_all   test_camera_estimator )
ADD_TEST( dbccl_test_manual_camera     dbccl_test_all   test_manual_camera )
ADD_TEST( dbccl_test_fast_mi           dbccl_test_all   test_fast_mi )
