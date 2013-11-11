#include <testlib/testlib_register.h>

DECLARE( test_load_edg );
DECLARE( test_sel );

void
register_tests()
{
  REGISTER( test_load_edg );
  REGISTER( test_sel );
}

DEFINE_MAIN;
