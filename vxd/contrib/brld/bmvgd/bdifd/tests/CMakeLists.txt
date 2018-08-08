# This is brld/bmvgd/bdifd/tests/CMakeLists.txt

add_executable ( bdifd_test_all
   # Driver source and utilities
   test_driver.cxx

   # The tests
   test_camera.cxx
)

target_link_libraries( bdifd_test_all bdifd testlib )

add_test( NAME  bdifd_test_camera COMMAND $<TARGET_FILE:bdifd_test_all> test_camera )

add_executable( bdifd_test_include test_include.cxx )

target_link_libraries( bdifd_test_include bdifd )
