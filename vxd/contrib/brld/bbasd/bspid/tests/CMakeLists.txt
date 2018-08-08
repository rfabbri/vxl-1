# This is basic/bspid/tests

add_executable ( bspid_test_all
  test_driver.cxx
  test_curve_distance.cxx
)

target_link_libraries( bspid_test_all bspid  bild_algo testlib )

add_test( NAME  bspid_test_curve_distance COMMAND $<TARGET_FILE:bspid_test_all> test_curve_distance )

add_executable( bspid_test_include test_include.cxx )
target_link_libraries( bspid_test_include bspid)
