INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/can/3d_curve_matching )

ADD_EXECUTABLE( crvmtch3D_test_all   
  test_driver.cxx  
  test_crvmtch3D.cxx
)

TARGET_LINK_LIBRARIES(crvmtch3D_test_all testlib vcl crvmtch3D)

ADD_TEST( crvmtch3D_test crvmtch3D_test_all)


