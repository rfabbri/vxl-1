# brl/btrk/bapl/tests/CMakeLists.txt


ADD_EXECUTABLE( bapl_test_all
  test_driver.cxx
  test_transform.cxx
  test_roi.cxx
  test_matcher.cxx
  test_lowe_pyramid_set.cxx
)

TARGET_LINK_LIBRARIES( bapl_test_all bapl testlib vil vnl brip)

ADD_TEST( bapl_test_transform         bapl_test_all test_transform )
ADD_TEST( bapl_test_roi               bapl_test_all test_roi )
ADD_TEST( bapl_test_matcher           bapl_test_all test_matcher )
ADD_TEST( bapl_test_lowe_pyramid_set  bapl_test_all test_lowe_pyramid_set )

ADD_EXECUTABLE( bapl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bapl_test_include bapl )
