include_directories( ${lems_SOURCE_DIR}/contrib/Gregistration )


add_executable( dt_test_all
  test_driver.cxx
  test_partial_derivatives.cxx
)

target_link_libraries( dt_test_all dt testlib vnl_algo)
add_test( dt_test_partial_derivatives dbgl_test_all test_partial_derivatives)


add_executable( dt_test_include test_include.cxx )
target_link_libraries( dt_test_include dt )
