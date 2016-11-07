#include <testlib/testlib_register.h>

DECLARE( test_tracker );
DECLARE( test_camera_estimator );
DECLARE( test_manual_camera );

void register_tests()
{
  REGISTER( test_camera_estimator );
  REGISTER( test_tracker );
  REGISTER( test_manual_camera );
}

DEFINE_MAIN;
