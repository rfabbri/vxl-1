#include <testlib/testlib_register.h>

DECLARE(test_explicit_splat_collection);
DECLARE(test_pizza_slice_symmetry);
DECLARE(test_basic_pizza_slice_splat_collection);
DECLARE(test_symmetry_worldpt_roster);
DECLARE(test_symmetry_splat_collection);
//DECLARE(test_pizza_slice_symmetry_representatives);
DECLARE(test_subset_splat_collection);
void
register_tests()
{
REGISTER(test_explicit_splat_collection);
REGISTER(test_pizza_slice_symmetry);
REGISTER(test_basic_pizza_slice_splat_collection);
REGISTER(test_symmetry_worldpt_roster);
REGISTER(test_symmetry_splat_collection);
//REGISTER(test_pizza_slice_symmetry_representatives);
REGISTER(test_subset_splat_collection);
}

DEFINE_MAIN;
