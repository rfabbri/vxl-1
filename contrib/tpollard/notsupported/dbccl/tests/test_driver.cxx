#include <testlib/testlib_register.h>

DECLARE( test_fast_mi );
DECLARE( test_aerial_tracker );
DECLARE( test_camera_estimator );
DECLARE( test_manual_camera );

void register_tests()
{
  REGISTER( test_fast_mi );
  REGISTER( test_camera_estimator );
  REGISTER( test_aerial_tracker );
  REGISTER( test_manual_camera );
}

DEFINE_MAIN;
