#include <testlib/testlib_register.h>


DECLARE( test_dbetrk_track);
DECLARE( test_dbetrk_edge);

void
register_tests()
{
  REGISTER( test_dbetrk_track);
  REGISTER( test_dbetrk_edge);
}

DEFINE_MAIN;

