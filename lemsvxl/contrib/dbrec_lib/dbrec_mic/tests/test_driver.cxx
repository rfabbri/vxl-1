#include <testlib/testlib_register.h>

DECLARE( test_mnist );
DECLARE( test_pca_util );

void
register_tests()
{
  REGISTER( test_mnist );
  REGISTER( test_pca_util );
}

DEFINE_MAIN;


