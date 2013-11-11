# contrib/biotree/biocts/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )

ADD_EXECUTABLE( biocts_test_all
  test_driver.cxx
  test_biocts.cxx
)

TARGET_LINK_LIBRARIES( biocts_test_all biocts testlib )

ADD_TEST(biocts_test biocts_test_all)

ADD_EXECUTABLE( biocts_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( biocts_test_include biocts )
