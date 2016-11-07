include_directories( ${lems_SOURCE_DIR}/contrib )

set(bioproj_test_src
  test_driver.cxx  
  test_bioproj_io.cxx
  test_bioproj_mem_io.cxx
  test_bioproj_filters.cxx
  test_error_function.cxx
  test_simpson_integral.cxx
)

aux_source_directory(Templates bioproj_test_src)

add_executable( bioproj_test_all ${bioproj_test_src})

add_test( bioproj_test_bioproj_io
          bioproj_test_all test_bioproj_io)

add_test( bioproj_test_bioproj_mem_io
          bioproj_test_all test_bioproj_mem_io)

add_test( bioproj_test_bioproj_filters
          bioproj_test_all test_bioproj_filters)

add_test( bioproj_test_error_function
          bioproj_test_all test_error_function)

add_test( bioproj_test_simpson_integral
          bioproj_test_all test_simpson_integral)

target_link_libraries(bioproj_test_all bioproj testlib vcl )


