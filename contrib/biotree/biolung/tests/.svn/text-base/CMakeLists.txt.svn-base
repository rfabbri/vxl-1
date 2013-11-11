# contrib/biotree/biolung/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )

ADD_EXECUTABLE( biolung_test_all
  test_driver.cxx
  test_biolung.cxx
)

TARGET_LINK_LIBRARIES( biolung_test_all biolung testlib )

ADD_TEST(biolung_test biolung_test_all)

ADD_EXECUTABLE( biolung_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( biolung_test_include biolung )
