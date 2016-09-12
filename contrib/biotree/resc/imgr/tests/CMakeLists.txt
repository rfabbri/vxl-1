include_directories( ${lems_SOURCE_DIR}/contrib )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )

add_executable( imgr_test_all
  test_driver.cxx
   test_scan_resource.cxx
   test_view_data.cxx
   test_image_view_3d.cxx
)

target_link_libraries( imgr_test_all imgr vpgl_algo dbgl dbgl_algo vnl testlib imgr_file_format)

add_test( imgr_test_view_data imgr_test_all test_view_data)

add_test( imgr_test_scan_resource 
          imgr_test_all 
          test_scan_resource 
          ${CMAKE_CURRENT_SOURCE_DIR}/test_data/18_al05_1770_\#\#\#\#.tif)

add_test( imgr_test_image_view_3d imgr_test_all test_image_view_3d)




