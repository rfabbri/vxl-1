#include <testlib/testlib_register.h>

DECLARE( test_perspective_camera );
DECLARE( test_xmvg_filter );
DECLARE( test_xmvg_no_noise_filter );
DECLARE( test_xmvg_gaussian_filter );
DECLARE( test_xmvg_composite_filters );
DECLARE( test_filter_response );
DECLARE( test_xmvg_point_filter);
DECLARE( test_xmvg_parallel_beam_filter_3d);
DECLARE( test_xmvg_gaussian_edge_detector );
DECLARE( test_xmvg_splat_filter );
DECLARE( test_xmvg_impulse_response );
DECLARE( test_xmvg_gaussian_filter_descriptor );

void register_tests()
{
  REGISTER( test_perspective_camera );
  REGISTER( test_xmvg_filter );
  REGISTER( test_xmvg_no_noise_filter );
  REGISTER( test_xmvg_gaussian_filter );
  REGISTER( test_xmvg_composite_filters );
  REGISTER( test_filter_response );
  REGISTER( test_xmvg_point_filter );
  REGISTER( test_xmvg_parallel_beam_filter_3d);
  REGISTER( test_xmvg_gaussian_edge_detector );
  REGISTER( test_xmvg_splat_filter );
  REGISTER( test_xmvg_impulse_response );
  REGISTER( test_xmvg_gaussian_filter_descriptor );
}

DEFINE_MAIN;
