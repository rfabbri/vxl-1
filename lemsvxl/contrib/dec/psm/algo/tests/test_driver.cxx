#include <testlib/testlib_register.h>

//DECLARE( test_psm_render_expected );
//DECLARE( test_psm_update );
//DECLARE( test_psm_track_rays );
//DECLARE( test_psm_triangle_scan_iterator_aa );
//DECLARE( test_psm_triangle_scan_iterator );
//DECLARE( test_psm_triangle_interpolation_iterator );
//DECLARE( test_psm_raytrace_operations );
//DECLARE( test_convert_to_boct );
//DECLARE( test_psm_condense_scene );
DECLARE( test_satellite_rays );


void register_tests()
{
  //REGISTER( test_psm_triangle_scan_iterator );
  //REGISTER( test_psm_triangle_scan_iterator_aa );
  //REGISTER( test_psm_triangle_interpolation_iterator );
  //REGISTER( test_psm_raytrace_operations );
  //REGISTER( test_convert_to_boct );
  //REGISTER( test_psm_condense_scene );
  REGISTER( test_satellite_rays );

  //REGISTER( test_psm_update );
  //REGISTER( test_psm_render_expected );
  //REGISTER(test_psm_track_rays );
}

DEFINE_MAIN;
