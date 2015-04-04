# This is /contrib/rfabbri/mw/algo/tests

ADD_EXECUTABLE (mw_algo_test_all
   # Driver source and utilities
   test_driver.cxx

   # The tests
   test_discrete_corresp_n_algo.cxx
   test_reprojection_errors.cxx
   test_stereo_band.cxx
   test_tangent_band.cxx
   test_util.cxx
   test_qualitative_epipolar.cxx
   test_cam.cxx
   test_sift_curve.cxx
   test_dbpro_process.cxx
   test_multiview_dbpro_process.cxx
   test_dbmcs_stereo_driver.cxx
   test_dvcpl_distmap_bundle_adjust.cxx
)

if(Boost_FOUND)
  TARGET_LINK_LIBRARIES(mw_algo_test_all mw_algo vpgl_algo mw_pro testlib ${Boost_LIBRARIES})
  INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )
else()
  TARGET_LINK_LIBRARIES(mw_algo_test_all mw_algo vpgl_algo mw_pro testlib)
endif()

#ADD_TEST( mw_algo_test_reprojection_errors mw_algo_test_all test_reprojection_errors )

ADD_TEST( mw_algo_test_discrete_corresp_n_algo mw_algo_test_all test_discrete_corresp_n_algo )
ADD_TEST( mw_algo_test_stereo_band mw_algo_test_all test_stereo_band )
ADD_TEST( mw_algo_test_tangent_band mw_algo_test_all test_tangent_band)
ADD_TEST( mw_algo_test_util mw_algo_test_all test_util )
ADD_TEST( mw_algo_test_cam mw_algo_test_all test_cam )
ADD_TEST( mw_algo_test_sift_curve mw_algo_test_all test_sift_curve )
ADD_TEST( mw_algo_test_dbpro_process mw_algo_test_all test_dbpro_process )
ADD_TEST( mw_algo_test_multiview_dbpro_process mw_algo_test_all test_multiview_dbpro_process )
ADD_TEST( mw_algo_test_dbmcs_stereo_driver mw_algo_test_all test_dbmcs_stereo_driver)
ADD_TEST( mw_algo_test_dvcpl_distmap_bundle_ajust mw_algo_test_all test_dvcpl_distmap_bundle_adjust )

ADD_EXECUTABLE( mw_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES(mw_algo_test_include mw_algo)

