# brcv/seg/dbbgm/tests/CMakeLists.txt
# Tests for dbbgm library

ADD_EXECUTABLE( dbbgm_test_all
  test_driver.cxx
  test_bg_model_speed.cxx
)

TARGET_LINK_LIBRARIES( dbbgm_test_all dbbgm bbgm dbsta_algo bsta_algo vul vil testlib )

ADD_TEST( dbbgm_test_bg_model_speed           dbbgm_test_all test_bg_model_speed )

ADD_EXECUTABLE( dbbgm_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( dbbgm_test_include dbbgm)
