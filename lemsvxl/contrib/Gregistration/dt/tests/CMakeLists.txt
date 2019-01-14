include_directories( ${LEMS_SOURCE_DIR}/contrib/Gregistration )


add_executable( dt_test_all
  test_driver.cxx
  test_partial_derivatives.cxx
)

target_link_libraries( dt_test_all dt testlib vnl_algo)
add_test( NAME  dt_test_partial_derivatives COMMAND $<TARGET_FILE:dbgl_test_all> test_partial_derivatives)


add_executable( dt_test_include test_include.cxx )
target_link_libraries( dt_test_include dt )
