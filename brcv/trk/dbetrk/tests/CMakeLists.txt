# brcv/trk/dbetrk/tests/CMakeLists.txt


add_executable( dbetrk_test_all
  test_driver.cxx
  test_dbetrk_track.cxx
  test_dbetrk_edge.cxx
)

target_link_libraries( dbetrk_test_all dbetrk testlib )
add_test(dbetrk_track  dbetrk_test_all test_dbetrk_track ) 
add_test(dbetrk_edge   dbetrk_test_all test_dbetrk_edge ) 


add_executable( dbetrk_test_include test_include.cxx )
target_link_libraries( dbetrk_test_include dbetrk )


