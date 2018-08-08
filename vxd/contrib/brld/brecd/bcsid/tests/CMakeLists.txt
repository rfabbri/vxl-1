# This is rec/bcsid/tests

add_executable ( bcsid_test_all
  test_driver.cxx
  test_curve_distance.cxx
)

target_link_libraries( bcsid_test_all bcsid sdetd_algo bild_algo vil_algo testlib )

add_test( NAME  bcsid_test_curve_distance COMMAND $<TARGET_FILE:bcsid_test_all> test_curve_distance )

add_executable( bcsid_test_include test_include.cxx )
target_link_libraries( bcsid_test_include bcsid)
