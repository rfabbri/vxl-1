# brl/bseg/blem/brcv/trk/dbinfo/tests/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/breye1 )

add_executable( dbinfo_test_all
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

target_link_libraries(dbinfo_test_all dbinfo_pro dbinfo bvis1 brip btol vgl_algo vil1 vnl_algo vsl vpl vbl vul testlib)

add_test( NAME dbinfo_test_feature_data    COMMAND $<TARGET_FILE:dbinfo_test_all>   test_feature_data)
add_test( NAME dbinfo_test_region_geometry COMMAND $<TARGET_FILE:dbinfo_test_all>   test_region_geometry)
add_test( NAME dbinfo_test_features        COMMAND $<TARGET_FILE:dbinfo_test_all>   test_features)
add_test( NAME dbinfo_test_observation     COMMAND $<TARGET_FILE:dbinfo_test_all>   test_observation)
add_test( NAME dbinfo_test_observation_generator COMMAND $<TARGET_FILE:dbinfo_test_all> test_observation_generator)
add_test( NAME dbinfo_test_observation_matcher COMMAND $<TARGET_FILE:dbinfo_test_all> test_observation_matcher)
add_test( NAME dbinfo_test_track_storage COMMAND $<TARGET_FILE:dbinfo_test_all> test_track_storage)
add_test( NAME dbinfo_test_multi_tracker COMMAND $<TARGET_FILE:dbinfo_test_all> test_multi_tracker)
add_test( NAME dbinfo_test_match_optimizer COMMAND $<TARGET_FILE:dbinfo_test_all> test_match_optimizer)
add_test( NAME dbinfo_test_osl COMMAND $<TARGET_FILE:dbinfo_test_all> test_osl)

add_executable( dbinfo_test_include test_include.cxx )
target_link_libraries( dbinfo_test_include dbinfo )
