# brcv/trk/dbctrk/tests/CMakeLists.txt


add_executable( dbctrk_test_all
  test_driver.cxx

)

target_link_libraries( dbctrk_test_all dbctrk testlib )


add_executable( dbctrk_test_include test_include.cxx )
target_link_libraries( dbctrk_test_include dbctrk )


