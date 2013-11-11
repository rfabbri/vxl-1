INCLUDE_DIRECTORIES( ${VXL_BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )

ADD_EXECUTABLE( psm_algo_test_all
  test_driver.cxx
  #test_psm_render_expected.cxx
  #test_psm_update.cxx
  #test_psm_track_rays.cxx
  test_psm_triangle_scan_iterator_aa.cxx
  test_psm_triangle_interpolation_iterator.cxx
  test_psm_raytrace_operations.cxx
  test_psm_convert_to_boct.cxx
  test_psm_triangle_scan_iterator.cxx
  test_psm_condense_scene.cxx
)

TARGET_LINK_LIBRARIES( psm_algo_test_all psm psm_algo hsds testlib vbl vgl vpgl vpgl_algo vil boct boxm)


#ADD_TEST( psm_render_expected ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all   test_psm_render_expected )
#ADD_TEST( psm_update  ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all   test_psm_update )
#ADD_TEST( psm_track_rays ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all  test_psm_track_rays )
#ADD_TEST( psm_compute_vis_implicit ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all  test_psm_compute_vis_implicit )
ADD_TEST( psm_triangle_scan_iterator_aa ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all  test_psm_triangle_scan_iterator_aa )
ADD_TEST( psm_triangle_interpolation_iterator ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all  test_psm_triangle_interpolation_iterator )
ADD_TEST( psm_raytrace_operations ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all  test_psm_raytrace_operations)
ADD_TEST( psm_convert_to_boct ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all  test_convert_to_boct)
ADD_TEST( psm_triangle_scan_iterator ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all test_psm_triangle_scan_iterator )
ADD_TEST( psm_condense_scene ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all test_psm_condense_scene )
ADD_TEST( satellite_rays ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all test_satellite_rays )




