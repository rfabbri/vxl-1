#include <testlib/testlib_register.h>

DECLARE ( test_patch_selectors );
DECLARE ( test_graph_algos );
DECLARE ( test_shock_patch_match );

void
register_tests()
{
  //REGISTER( test_patch_selectors );
  //REGISTER( test_graph_algos );
  REGISTER( test_shock_patch_match );
}

DEFINE_MAIN;
