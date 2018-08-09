include_directories( ${lems_SOURCE_DIR}/contrib/biotree/proc/ )

add_executable( bioproc_util_test_all   
  test_driver.cxx  
  test_proc_filter_stats.cxx  
  )
  
#set(bioproc_det_test_src
#  test_driver.cxx  
#  test_proc_filter_stats.cxx
#)

#add_executable( bioproc_util_test_all ${bioproc_util_test_src})

add_test( NAME  bioproc_test_proc_filter_stats COMMAND $<TARGET_FILE:bioproc_det_test_all>
          test_proc_filter_stats)

target_link_libraries(bioproc_util_test_all bioproc_util testlib vcl )


