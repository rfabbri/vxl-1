# brcv/seg/dbbgm/tests/CMakeLists.txt
# Tests for dbbgm library

add_executable( dbbgm_test_all
  test_driver.cxx
  test_bg_model_speed.cxx
)

target_link_libraries( dbbgm_test_all dbbgm bbgm dbsta_algo bsta_algo vul vil testlib )

add_test( NAME  dbbgm_test_bg_model_speed           COMMAND $<TARGET_FILE:dbbgm_test_all> test_bg_model_speed )

add_executable( dbbgm_test_include test_include.cxx )

target_link_libraries( dbbgm_test_include dbbgm)
