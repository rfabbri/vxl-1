#include <testlib/testlib_register.h>


DECLARE( test_gaussian_stats );
DECLARE( test_adaptive_updater );
DECLARE( test_sinkhorn );


void
register_tests()
{
  REGISTER( test_gaussian_stats );
  REGISTER( test_adaptive_updater );
  REGISTER( test_sinkhorn );

}

DEFINE_MAIN;



