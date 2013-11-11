# brcv/trk/dbetrk/tests/CMakeLists.txt


ADD_EXECUTABLE( dbetrk_test_all
  test_driver.cxx
  test_dbetrk_track.cxx
  test_dbetrk_edge.cxx
)

TARGET_LINK_LIBRARIES( dbetrk_test_all dbetrk testlib )
ADD_TEST(dbetrk_track  dbetrk_test_all test_dbetrk_track ) 
ADD_TEST(dbetrk_edge   dbetrk_test_all test_dbetrk_edge ) 


ADD_EXECUTABLE( dbetrk_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbetrk_test_include dbetrk )


