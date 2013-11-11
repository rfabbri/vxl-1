INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )

SET(bioproj_test_src
  test_driver.cxx  
  test_bioproj_io.cxx
  test_bioproj_mem_io.cxx
  test_bioproj_filters.cxx
  test_error_function.cxx
  test_simpson_integral.cxx
)

AUX_SOURCE_DIRECTORY(Templates bioproj_test_src)

ADD_EXECUTABLE( bioproj_test_all ${bioproj_test_src})

ADD_TEST( bioproj_test_bioproj_io
          bioproj_test_all test_bioproj_io)

ADD_TEST( bioproj_test_bioproj_mem_io
          bioproj_test_all test_bioproj_mem_io)

ADD_TEST( bioproj_test_bioproj_filters
          bioproj_test_all test_bioproj_filters)

ADD_TEST( bioproj_test_error_function
          bioproj_test_all test_error_function)

ADD_TEST( bioproj_test_simpson_integral
          bioproj_test_all test_simpson_integral)

TARGET_LINK_LIBRARIES(bioproj_test_all bioproj testlib vcl )


