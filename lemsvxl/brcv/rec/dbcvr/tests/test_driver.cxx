#include <testlib/testlib_register.h>


DECLARE( test_cvmatch );
DECLARE( test_cvmatch_even );
DECLARE( test_clsd_cvmatch );
DECLARE( test_interp_cvmatch );
DECLARE( test_clsd_interp_cvmatch );

void
register_tests()
{
  REGISTER( test_cvmatch );
  REGISTER( test_cvmatch_even );
  REGISTER( test_clsd_cvmatch );
  REGISTER( test_interp_cvmatch );
  REGISTER( test_clsd_interp_cvmatch );
}

DEFINE_MAIN;


