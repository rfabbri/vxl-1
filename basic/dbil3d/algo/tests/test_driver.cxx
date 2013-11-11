#include <testlib/testlib_register.h>


DECLARE( test_dbil3d_finite_differences);
DECLARE( test_dbil3d_finite_second_differences);


void
register_tests()
{
  REGISTER( test_dbil3d_finite_differences);
  REGISTER( test_dbil3d_finite_second_differences);
}

DEFINE_MAIN;
