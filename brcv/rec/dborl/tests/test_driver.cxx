#include <testlib/testlib_register.h>

DECLARE( test_image_object );
DECLARE( test_dataset );
DECLARE( test_category_info );
DECLARE( test_index );
DECLARE( test_evaluation );

void
register_tests()
{
  REGISTER( test_image_object );
  REGISTER( test_dataset );
  REGISTER( test_category_info );
  REGISTER( test_index );
  REGISTER( test_evaluation );
}

DEFINE_MAIN;


