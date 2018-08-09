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


#add_test( NAME  psm_render_expected COMMAND $<TARGET_FILE:psm_algo_test_all>   test_psm_render_expected )
#add_test( NAME  psm_update  COMMAND $<TARGET_FILE:psm_algo_test_all>   test_psm_update )
#add_test( NAME  psm_track_rays COMMAND $<TARGET_FILE:psm_algo_test_all>  test_psm_track_rays )
#add_test( NAME  psm_compute_vis_implicit COMMAND $<TARGET_FILE:psm_algo_test_all>  test_psm_compute_vis_implicit )
add_test( NAME  psm_triangle_scan_iterator_aa COMMAND $<TARGET_FILE:psm_algo_test_all>  test_psm_triangle_scan_iterator_aa )
add_test( NAME  psm_triangle_interpolation_iterator COMMAND $<TARGET_FILE:psm_algo_test_all>  test_psm_triangle_interpolation_iterator )
add_test( NAME  psm_raytrace_operations COMMAND $<TARGET_FILE:psm_algo_test_all>  test_psm_raytrace_operations)
add_test( NAME  psm_convert_to_boct COMMAND $<TARGET_FILE:psm_algo_test_all>  test_convert_to_boct)
add_test( NAME  psm_triangle_scan_iterator COMMAND $<TARGET_FILE:psm_algo_test_all> test_psm_triangle_scan_iterator )
add_test( NAME  psm_condense_scene COMMAND $<TARGET_FILE:psm_algo_test_all> test_psm_condense_scene )
add_test( NAME  satellite_rays COMMAND $<TARGET_FILE:psm_algo_test_all> test_satellite_rays )




