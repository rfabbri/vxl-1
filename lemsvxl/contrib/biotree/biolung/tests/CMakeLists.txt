# contrib/biotree/biolung/tests/CMakeLists.txt

include_directories( ${lems_SOURCE_DIR}/contrib )

add_executable( biolung_test_all
  test_driver.cxx
  test_biolung.cxx
)

target_link_libraries( biolung_test_all biolung testlib )

add_test( NAME biolung_test COMMAND $<TARGET_FILE:biolung_test_all>)

add_executable( biolung_test_include test_include.cxx )
target_link_libraries( biolung_test_include biolung )
