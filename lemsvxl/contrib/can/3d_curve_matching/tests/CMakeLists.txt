include_directories( ${lems_SOURCE_DIR}/contrib/can/3d_curve_matching )

add_executable( crvmtch3D_test_all   
  test_driver.cxx  
  test_crvmtch3D.cxx
)

target_link_libraries(crvmtch3D_test_all testlib vcl crvmtch3D)

add_test( NAME  crvmtch3D_test COMMAND $<TARGET_FILE:crvmtch3D_test_all>)


