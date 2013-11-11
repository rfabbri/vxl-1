ADD_EXECUTABLE( dbccl_test_all
  test_driver.cxx
  test_manual_camera.cxx
)

TARGET_LINK_LIBRARIES( dbccl_test_all dbccl vgl vnl vil vidl1 vpl testlib )
ADD_TEST( dbccl_test_manual_camera     dbccl_test_all   test_manual_camera )
