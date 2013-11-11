# basic/dbsol/tests/CMakeLists.txt


ADD_EXECUTABLE( dbsol_test_all
  test_driver.cxx
  test_interp_curve_2d.cxx
  test_interp_curve_3d.cxx
  test_file_io.cxx
)

TARGET_LINK_LIBRARIES( dbsol_test_all dbsol testlib )

ADD_TEST( dbsol_test_interp_curve_2d  dbsol_test_all test_interp_curve_2d )
ADD_TEST( dbsol_test_interp_curve_3d  dbsol_test_all test_interp_curve_3d )
ADD_TEST( dbsol_test_file_io dbsol_test_all test_file_io )

ADD_EXECUTABLE( dbsol_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbsol_test_include dbsol vsl )
