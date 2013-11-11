# brl/bseg/blem/brcv/trk/dbinfo/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/breye1 )

ADD_EXECUTABLE( dbinfo_test_all
  test_driver.cxx
  test_feature_data.cxx
  test_region_geometry.cxx
  test_features.cxx
  test_observation.cxx
  test_observation_generator.cxx
  test_observation_matcher.cxx
  test_multi_tracker.cxx
  test_track_storage.cxx
  test_match_optimizer.cxx
  test_osl.cxx
)

TARGET_LINK_LIBRARIES(dbinfo_test_all dbinfo_pro dbinfo bvis1 brip btol vgl_algo vil1 vnl_algo vsl vpl vbl vul testlib)

ADD_TEST(dbinfo_test_feature_data    dbinfo_test_all   test_feature_data)
ADD_TEST(dbinfo_test_region_geometry dbinfo_test_all   test_region_geometry)
ADD_TEST(dbinfo_test_features        dbinfo_test_all   test_features)
ADD_TEST(dbinfo_test_observation     dbinfo_test_all   test_observation)
ADD_TEST(dbinfo_test_observation_generator dbinfo_test_all test_observation_generator)
ADD_TEST(dbinfo_test_observation_matcher dbinfo_test_all test_observation_matcher)
ADD_TEST(dbinfo_test_track_storage dbinfo_test_all test_track_storage)
ADD_TEST(dbinfo_test_multi_tracker dbinfo_test_all test_multi_tracker)
ADD_TEST(dbinfo_test_match_optimizer dbinfo_test_all test_match_optimizer)
ADD_TEST(dbinfo_test_osl dbinfo_test_all test_osl)

ADD_EXECUTABLE( dbinfo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbinfo_test_include dbinfo )
