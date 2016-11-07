#include <testlib/testlib_register.h>

DECLARE(test_worldpt_transformation);
DECLARE(test_explicit_neighborhood_structure);
DECLARE(test_worldpt_field);
//DECLARE(test_grid_worldpt_roster);
  
void
register_tests()
{
  REGISTER(test_worldpt_transformation);
  REGISTER(test_explicit_neighborhood_structure);
  REGISTER(test_worldpt_field);
  //  REGISTER(test_grid_worldpt_roster);
}

DEFINE_MAIN;
