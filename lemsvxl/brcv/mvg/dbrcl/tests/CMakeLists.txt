add_executable( dbrcl_test_all
  test_driver.cxx

  test_constrained_camera.cxx
)

target_link_libraries( dbrcl_test_all dbrcl vpl testlib )
add_test( NAME  dbrcl_test_constrained_camera    COMMAND $<TARGET_FILE:dbrcl_test_all>  test_constrained_camera )
