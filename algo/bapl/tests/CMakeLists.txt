# brl/btrk/bapl/tests/CMakeLists.txt


add_executable( bapl_test_all
  test_driver.cxx
  test_transform.cxx
  test_roi.cxx
  test_matcher.cxx
  test_lowe_pyramid_set.cxx
)

target_link_libraries( bapl_test_all bapl testlib vil vnl brip)

add_test( bapl_test_transform         bapl_test_all test_transform )
add_test( bapl_test_roi               bapl_test_all test_roi )
add_test( bapl_test_matcher           bapl_test_all test_matcher )
add_test( bapl_test_lowe_pyramid_set  bapl_test_all test_lowe_pyramid_set )

add_executable( bapl_test_include test_include.cxx )
target_link_libraries( bapl_test_include bapl )
