# contrib/biotree/xmvg/tests/CMakeLists.txt
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${lems_SOURCE_DIR}/contrib )

add_executable( xmvg_test_all   
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

target_link_libraries(xmvg_test_all vbl vgl testlib vcl xmvg xscan)

add_test( imgr_test_perspective_camera xmvg_test_all test_perspective_camera )

add_test( xmvg_test_filter   xmvg_test_all test_xmvg_filter)

add_test( xmvg_test_no_noise_filter   xmvg_test_all test_xmvg_no_noise_filter)

add_test( xmvg_test_gaussian_filter   xmvg_test_all test_xmvg_gaussian_filter)

add_test( xmvg_test_composite_filters   xmvg_test_all test_xmvg_composite_filters)

add_test( xmvg_test_filter_response   xmvg_test_all test_xmvg_filter_response)

add_test( xmvg_point_filter           xmvg_test_all test_xmvg_point_filter)

add_test( xmvg_gaussian_edge_detector  xmvg_test_all test_xmvg_gaussian_edge_detector)

add_test( xmvg_splat_filter  xmvg_test_all test_xmvg_splat_filter)

add_test( xmvg_impulse_response  xmvg_test_all test_xmvg_impulse_response)

add_test(xmvg_test_gaussian_filter_descriptor  xmvg_test_all test_xmvg_gaussian_filter_descriptor)

add_executable( xmvg_test_include test_include.cxx )
target_link_libraries( xmvg_test_include  xmvg )

