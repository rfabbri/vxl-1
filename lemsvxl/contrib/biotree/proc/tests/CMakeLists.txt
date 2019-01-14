include_directories( ${LEMS_SOURCE_DIR}/contrib )

set(bioproc_test_src
  test_driver.cxx  
  test_bioproc_dummy_splat_resource.cxx
  test_bioproc_filtering_proc.cxx
#  test_bioproc_splr_proc.cxx
#  test_bioproc_splr_proc_splats.cxx
  bioproc_tests_modified_no_noise_filter_3d.cxx
  bioproc_tests_modified_no_noise_filter_3d.h
)

aux_source_directory(Templates bioproc_test_src)

add_executable( bioproc_test_all ${bioproc_test_src})

add_test( NAME  bioproc_test_splat_resource
          COMMAND $<TARGET_FILE:bioproc_test_all>
          test_bioproc_dummy_splat_resource
          ${CMAKE_CURRENT_SOURCE_DIR}/test_data/toy\#\#\#\#.tif)

add_test( NAME  bioproc_test_filtering_proc
          COMMAND $<TARGET_FILE:bioproc_test_all>
          test_bioproc_filtering_proc
          ${CMAKE_CURRENT_SOURCE_DIR}/test_data/sanity00\#\#.tif)

target_link_libraries(bioproc_test_all bioproc testlib vcl )


