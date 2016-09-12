add_executable( dbavl_test_all
  test_driver.cxx
  test_tracker.cxx
  test_camera_estimator.cxx
  test_manual_camera.cxx
)

target_link_libraries( dbavl_test_all dbavl vgl vnl vil vidl1 vpl testlib )
add_test( dbavl_test_tracker           dbavl_test_all   test_tracker )
add_test( dbavl_test_camera_estimator  dbavl_test_all   test_camera_estimator )
add_test( dbavl_test_manual_camera     dbavl_test_all   test_manual_camera )
