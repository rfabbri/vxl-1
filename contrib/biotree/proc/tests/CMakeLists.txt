INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib )

SET(bioproc_test_src
  test_driver.cxx  
  test_bioproc_dummy_splat_resource.cxx
  test_bioproc_filtering_proc.cxx
#  test_bioproc_splr_proc.cxx
#  test_bioproc_splr_proc_splats.cxx
  bioproc_tests_modified_no_noise_filter_3d.cxx
  bioproc_tests_modified_no_noise_filter_3d.h
)

AUX_SOURCE_DIRECTORY(Templates bioproc_test_src)

ADD_EXECUTABLE( bioproc_test_all ${bioproc_test_src})

ADD_TEST( bioproc_test_splat_resource
          bioproc_test_all
          test_bioproc_dummy_splat_resource
          ${CMAKE_CURRENT_SOURCE_DIR}/test_data/toy\#\#\#\#.tif)

ADD_TEST( bioproc_test_filtering_proc
          bioproc_test_all
          test_bioproc_filtering_proc
          ${CMAKE_CURRENT_SOURCE_DIR}/test_data/sanity00\#\#.tif)

TARGET_LINK_LIBRARIES(bioproc_test_all bioproc testlib vcl )


