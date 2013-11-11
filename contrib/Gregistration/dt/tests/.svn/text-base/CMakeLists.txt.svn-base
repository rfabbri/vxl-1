INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/Gregistration )


ADD_EXECUTABLE( dt_test_all
  test_driver.cxx
  test_partial_derivatives.cxx
)

TARGET_LINK_LIBRARIES( dt_test_all dt testlib vnl_algo)
ADD_TEST( dt_test_partial_derivatives dbgl_test_all test_partial_derivatives)


ADD_EXECUTABLE( dt_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dt_test_include dt )
