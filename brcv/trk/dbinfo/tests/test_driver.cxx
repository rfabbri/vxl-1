#include <testlib/testlib_register.h>


DECLARE(test_feature_data);
DECLARE(test_region_geometry);
DECLARE(test_features);
DECLARE(test_observation);
DECLARE(test_observation_generator);
DECLARE(test_observation_matcher);
DECLARE(test_multi_tracker);
DECLARE(test_track_storage);
DECLARE(test_match_optimizer);
DECLARE(test_osl);

void
register_tests()
{
  //REGISTER(test_feature_data);
  //REGISTER(test_region_geometry);
  REGISTER(test_features);
  //REGISTER(test_observation);
  //REGISTER(test_observation_generator);
  //REGISTER(test_observation_matcher);
  //REGISTER(test_multi_tracker);
  //REGISTER(test_track_storage);
  //REGISTER(test_match_optimizer);
  //REGISTER(test_osl);
}

DEFINE_MAIN;
