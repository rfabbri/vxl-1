#include <testlib/testlib_register.h>

DECLARE( test_type_id_factory );
DECLARE( test_part );
DECLARE( test_primitive_factory );
DECLARE( test_gaussian_primitive );
DECLARE( test_hierarchy );
DECLARE( test_part_context );
DECLARE( test_compositor );
DECLARE( test_image_learner_visitors );
DECLARE( test_visitors );
DECLARE( test_models );
DECLARE( test_part_selector );
DECLARE( test_rot_inv );

void
register_tests()
{
  REGISTER( test_type_id_factory );
  REGISTER( test_part );
  REGISTER( test_primitive_factory );
  REGISTER( test_gaussian_primitive );
  REGISTER( test_hierarchy );
  REGISTER( test_part_context );
  REGISTER( test_compositor );
  REGISTER( test_image_learner_visitors );
  REGISTER( test_visitors );
  REGISTER( test_models );
  REGISTER( test_part_selector );
  REGISTER( test_rot_inv );
}

DEFINE_MAIN;


