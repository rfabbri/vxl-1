INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/biotree/proc/ )

ADD_EXECUTABLE( bioproc_util_test_all   
  test_driver.cxx  
  test_proc_filter_stats.cxx  
  )
  
#SET(bioproc_det_test_src
#  test_driver.cxx  
#  test_proc_filter_stats.cxx
#)

#ADD_EXECUTABLE( bioproc_util_test_all ${bioproc_util_test_src})

ADD_TEST( bioproc_test_proc_filter_stats
          bioproc_det_test_all
          test_proc_filter_stats)

TARGET_LINK_LIBRARIES(bioproc_util_test_all bioproc_util testlib vcl )


