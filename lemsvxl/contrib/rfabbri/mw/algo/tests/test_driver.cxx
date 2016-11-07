#include <testlib/testlib_register.h>

DECLARE( test_stereo_band );
DECLARE( test_discrete_corresp_n_algo );
DECLARE( test_util );
DECLARE( test_cam );
DECLARE( test_qualitative_epipolar );
DECLARE( test_tangent_band );
DECLARE( test_sift_curve );
DECLARE( test_dbpro_process );
DECLARE( test_multiview_dbpro_process );
DECLARE( test_dbmcs_stereo_driver);
DECLARE( test_dvcpl_distmap_bundle_adjust );
//DECLARE( test_curve_stereo_batch );
//DECLARE( test_reprojection_errors );

void
register_tests()
{
  REGISTER( test_stereo_band );
  REGISTER( test_discrete_corresp_n_algo );
  REGISTER( test_util );
  REGISTER( test_qualitative_epipolar );
  REGISTER( test_cam );
  REGISTER( test_tangent_band );
  REGISTER( test_sift_curve );
  REGISTER( test_dbpro_process );
  REGISTER( test_multiview_dbpro_process );
  REGISTER( test_dbmcs_stereo_driver);
  REGISTER( test_dvcpl_distmap_bundle_adjust );
//  REGISTER( test_curve_stereo_batch );
//   REGISTER( test_reprojection_errors );
}

DEFINE_MAIN;
