add_executable( dbccl_test_all
  test_driver.cxx
  test_manual_camera.cxx
)

target_link_libraries( dbccl_test_all dbccl vgl vnl vil vidl1 vpl testlib )
add_test( NAME  dbccl_test_manual_camera     COMMAND $<TARGET_FILE:dbccl_test_all>   test_manual_camera )
