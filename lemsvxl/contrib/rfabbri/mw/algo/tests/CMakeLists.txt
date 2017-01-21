# This is /contrib/rfabbri/mw/algo/tests

add_executable (mw_algo_test_all
   # Driver source and utilities
   test_driver.cxx

   # The tests
   #    test_discrete_corresp_n_algo.cxx
   #    test_reprojection_errors.cxx
   #    test_stereo_band.cxx
   #    test_tangent_band.cxx
   test_util.cxx
   #    test_qualitative_epipolar.cxx
   test_cam.cxx
   test_sift_curve.cxx        # TODO Under development
   test_bprod_process.cxx
   test_multiview_bprod_process.cxx
   test_bmcsd_stereo_driver.cxx
   #    test_dvcpl_distmap_bundle_adjust.cxx
)

#add_test( mw_algo_test_reprojection_errors mw_algo_test_all test_reprojection_errors )

add_test( mw_algo_test_discrete_corresp_n_algo mw_algo_test_all test_discrete_corresp_n_algo )
#add_test( mw_algo_test_stereo_band mw_algo_test_all test_stereo_band )
#add_test( mw_algo_test_tangent_band mw_algo_test_all test_tangent_band)
add_test( mw_algo_test_util mw_algo_test_all test_util )
#add_test( mw_algo_test_cam mw_algo_test_all test_cam )
#add_test( mw_algo_test_sift_curve mw_algo_test_all test_sift_curve )
#add_test( mw_algo_test_bprod_process mw_algo_test_all test_bprod_process )
#add_test( mw_algo_test_multiview_bprod_process mw_algo_test_all test_multiview_bprod_process )
#add_test( mw_algo_test_bmcsd_stereo_driver mw_algo_test_all test_bmcsd_stereo_driver)
#add_test( mw_algo_test_dvcpl_distmap_bundle_ajust mw_algo_test_all test_dvcpl_distmap_bundle_adjust )

add_executable( mw_algo_test_include test_include.cxx )



find_package( Boost 1.35.0 COMPONENTS iostreams)
if(Boost_FOUND)
  target_link_libraries(mw_algo_test_all mw_algo testlib bmcsd_algo bprod ${Boost_LIBRARIES})
  include_directories(${Boost_INCLUDE_DIRS})
else()
  target_link_libraries(mw_algo_test_all mw_algo testlib bmcsd_algo bprod )
endif()

