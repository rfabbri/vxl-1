INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )

ADD_EXECUTABLE( imgr_test_all
  test_driver.cxx
   test_scan_resource.cxx
   test_view_data.cxx
   test_image_view_3d.cxx
)

TARGET_LINK_LIBRARIES( imgr_test_all imgr vpgl_algo dbgl dbgl_algo vnl testlib imgr_file_format)

ADD_TEST( imgr_test_view_data imgr_test_all test_view_data)

ADD_TEST( imgr_test_scan_resource 
          imgr_test_all 
          test_scan_resource 
          ${CMAKE_CURRENT_SOURCE_DIR}/test_data/18_al05_1770_\#\#\#\#.tif)

ADD_TEST( imgr_test_image_view_3d imgr_test_all test_image_view_3d)




