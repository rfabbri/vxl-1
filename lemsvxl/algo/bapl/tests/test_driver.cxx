#include <testlib/testlib_register.h>


DECLARE( test_transform );
DECLARE( test_roi );
DECLARE( test_matcher );
DECLARE( test_lowe_pyramid_set );

void
register_tests()
{
  REGISTER( test_transform );
  REGISTER( test_roi );
  REGISTER( test_matcher );
  REGISTER( test_lowe_pyramid_set );
}

DEFINE_MAIN;


