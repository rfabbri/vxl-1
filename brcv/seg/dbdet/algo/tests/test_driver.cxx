#include <testlib/testlib_register.h>

DECLARE( test_load_edg );
DECLARE( test_sel );
DECLARE( test_edgemap );

void
register_tests()
{
  REGISTER( test_load_edg );
  REGISTER( test_sel );
  REGISTER( test_edgemap );
}

DEFINE_MAIN;
