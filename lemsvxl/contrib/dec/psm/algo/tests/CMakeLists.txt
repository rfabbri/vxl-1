include_directories( ${VXL_BRL_INCLUDE_DIR}/bseg )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc )
include_directories( ${VXL_GEL_INCLUDE_DIR}/mrc/vpgl )

add_executable( psm_algo_test_all
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

target_link_libraries( psm_algo_test_all psm psm_algo hsds testlib vbl vgl vpgl vpgl_algo vil boct boxm)


#add_test( psm_render_expected ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all   test_psm_render_expected )
#add_test( psm_update  ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all   test_psm_update )
#add_test( psm_track_rays ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all  test_psm_track_rays )
#add_test( psm_compute_vis_implicit ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all  test_psm_compute_vis_implicit )
add_test( psm_triangle_scan_iterator_aa ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all  test_psm_triangle_scan_iterator_aa )
add_test( psm_triangle_interpolation_iterator ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all  test_psm_triangle_interpolation_iterator )
add_test( psm_raytrace_operations ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all  test_psm_raytrace_operations)
add_test( psm_convert_to_boct ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all  test_convert_to_boct)
add_test( psm_triangle_scan_iterator ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all test_psm_triangle_scan_iterator )
add_test( psm_condense_scene ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all test_psm_condense_scene )
add_test( satellite_rays ${EXECUTABLE_OUTPUT_PATH}/psm_algo_test_all test_satellite_rays )




