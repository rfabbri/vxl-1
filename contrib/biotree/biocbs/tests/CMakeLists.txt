# contrib/biotree/biocbs/tests/CMakeLists.txt

INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )

ADD_EXECUTABLE( biocbs_test_all
  test_driver.cxx
  test_biocbs.cxx
)

TARGET_LINK_LIBRARIES( biocbs_test_all biocbs testlib )

ADD_TEST(biocbs_test biocbs_test_all)

ADD_EXECUTABLE( biocbs_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( biocbs_test_include biocbs )
