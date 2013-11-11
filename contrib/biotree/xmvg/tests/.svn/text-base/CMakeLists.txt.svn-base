# contrib/biotree/xmvg/tests/CMakeLists.txt
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )

ADD_EXECUTABLE( xmvg_test_all   
  test_driver.cxx  
  test_xmvg_filter.cxx  
  test_perspective_camera.cxx
  test_xmvg_no_noise_filter.cxx
  test_xmvg_gaussian_filter.cxx
  test_xmvg_composite_filters.cxx 
  test_xmvg_filter_response.cxx
  test_xmvg_point_filter.cxx
  test_xmvg_parallel_beam_filter_3d.cxx
  test_xmvg_gaussian_edge_detector.cxx
  test_xmvg_splat_filter.cxx
  test_xmvg_impulse_response.cxx
  test_xmvg_gaussian_filter_descriptor.cxx
)

TARGET_LINK_LIBRARIES(xmvg_test_all vbl vgl testlib vcl xmvg xscan)

ADD_TEST( imgr_test_perspective_camera xmvg_test_all test_perspective_camera )

ADD_TEST( xmvg_test_filter   xmvg_test_all test_xmvg_filter)

ADD_TEST( xmvg_test_no_noise_filter   xmvg_test_all test_xmvg_no_noise_filter)

ADD_TEST( xmvg_test_gaussian_filter   xmvg_test_all test_xmvg_gaussian_filter)

ADD_TEST( xmvg_test_composite_filters   xmvg_test_all test_xmvg_composite_filters)

ADD_TEST( xmvg_test_filter_response   xmvg_test_all test_xmvg_filter_response)

ADD_TEST( xmvg_point_filter           xmvg_test_all test_xmvg_point_filter)

ADD_TEST( xmvg_gaussian_edge_detector  xmvg_test_all test_xmvg_gaussian_edge_detector)

ADD_TEST( xmvg_splat_filter  xmvg_test_all test_xmvg_splat_filter)

ADD_TEST( xmvg_impulse_response  xmvg_test_all test_xmvg_impulse_response)

ADD_TEST(xmvg_test_gaussian_filter_descriptor  xmvg_test_all test_xmvg_gaussian_filter_descriptor)

ADD_EXECUTABLE( xmvg_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( xmvg_test_include  xmvg )

