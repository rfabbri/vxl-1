# contrib/biotree/biocts/tests/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/contrib )

add_executable( biocts_test_all
  test_driver.cxx
  test_biocts.cxx
)

target_link_libraries( biocts_test_all biocts testlib )

add_test( NAME biocts_test COMMAND $<TARGET_FILE:biocts_test_all>)

add_executable( biocts_test_include test_include.cxx )

target_link_libraries( biocts_test_include biocts )
