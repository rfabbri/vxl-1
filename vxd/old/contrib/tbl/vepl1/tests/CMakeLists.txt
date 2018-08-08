# tbl/vepl1/tests/CMakeLists.txt

add_definitions(-DINSTANTIATE_TEMPLATES)

add_executable( vepl1_test_all
  test_driver.cxx        test_driver.h

  vepl1_test_threshold.cxx
  vepl1_test_erode_disk.cxx
)
TARGET_link_libraries( vepl1_test_all vepl1 vil1 testlib )

add_test( NAME vepl1_test_threshold  COMMAND $<TARGET_FILE:vepl1_test_all> vepl1_test_threshold)
add_test( NAME vepl1_test_erode_disk COMMAND $<TARGET_FILE:vepl1_test_all> vepl1_test_erode_disk)

add_executable( vepl1_test_include test_include.cxx )
TARGET_link_libraries( vepl1_test_include vepl1 )
add_executable( vepl1_test_template_include test_template_include.cxx )
TARGET_link_libraries( vepl1_test_template_include vepl1 )
