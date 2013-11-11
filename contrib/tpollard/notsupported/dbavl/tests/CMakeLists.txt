ADD_EXECUTABLE( dbavl_test_all
  test_driver.cxx
  test_tracker.cxx
  test_camera_estimator.cxx
  test_manual_camera.cxx
)

TARGET_LINK_LIBRARIES( dbavl_test_all dbavl vgl vnl vil vidl1 vpl testlib )
ADD_TEST( dbavl_test_tracker           dbavl_test_all   test_tracker )
ADD_TEST( dbavl_test_camera_estimator  dbavl_test_all   test_camera_estimator )
ADD_TEST( dbavl_test_manual_camera     dbavl_test_all   test_manual_camera )
