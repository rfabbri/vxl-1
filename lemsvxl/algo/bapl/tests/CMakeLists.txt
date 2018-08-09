# brl/btrk/bapl/tests/CMakeLists.txt


add_executable( bapl_test_all
  test_driver.cxx
  test_transform.cxx
  test_roi.cxx
  test_matcher.cxx
  test_lowe_pyramid_set.cxx
)

target_link_libraries( bapl_test_all bapl testlib vil vnl brip)

add_test( NAME  bapl_test_transform         COMMAND $<TARGET_FILE:bapl_test_all> test_transform )
add_test( NAME  bapl_test_roi               COMMAND $<TARGET_FILE:bapl_test_all> test_roi )
add_test( NAME  bapl_test_matcher           COMMAND $<TARGET_FILE:bapl_test_all> test_matcher )
add_test( NAME  bapl_test_lowe_pyramid_set  COMMAND $<TARGET_FILE:bapl_test_all> test_lowe_pyramid_set )

add_executable( bapl_test_include test_include.cxx )
target_link_libraries( bapl_test_include bapl )
