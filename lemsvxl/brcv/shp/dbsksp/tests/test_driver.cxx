#include <testlib/testlib_register.h>


DECLARE( test_shock_graph );
DECLARE( test_shapelet );
DECLARE( test_xshock_fragment );

void
register_tests()
{
  REGISTER( test_shock_graph );
  REGISTER( test_shapelet );
  REGISTER( test_xshock_fragment );
}

DEFINE_MAIN;


