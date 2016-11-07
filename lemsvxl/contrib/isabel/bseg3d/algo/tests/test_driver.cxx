#include <testlib/testlib_register.h>

DECLARE( test_gmm_l2norm );
DECLARE( test_merge_mixtures );
DECLARE( test_gauss_multiplier );
DECLARE( test_windows );
void register_tests()
{
  REGISTER( test_gmm_l2norm );
  REGISTER( test_merge_mixtures );
  REGISTER( test_gauss_multiplier );
  REGISTER( test_windows );
}

DEFINE_MAIN;
