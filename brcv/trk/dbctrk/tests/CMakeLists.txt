# brcv/trk/dbctrk/tests/CMakeLists.txt


ADD_EXECUTABLE( dbctrk_test_all
  test_driver.cxx

)

TARGET_LINK_LIBRARIES( dbctrk_test_all dbctrk testlib )


ADD_EXECUTABLE( dbctrk_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dbctrk_test_include dbctrk )


