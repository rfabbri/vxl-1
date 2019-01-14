# contrib/biotree/biocbs/tests/CMakeLists.txt

include_directories( ${LEMS_SOURCE_DIR}/contrib )

add_executable( biocbs_test_all
  test_driver.cxx
  test_biocbs.cxx
)

target_link_libraries( biocbs_test_all biocbs testlib )

add_test( NAME biocbs_test COMMAND $<TARGET_FILE:biocbs_test_all>)

add_executable( biocbs_test_include test_include.cxx )

target_link_libraries( biocbs_test_include biocbs )
