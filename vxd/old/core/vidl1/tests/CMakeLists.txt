# This is core/vidl1/tests/CMakeLists.txt

link_libraries( vidl1 )

add_executable( vidl1_test_all vidl1_test.cxx )
add_test( NAME  vidl1_test  COMMAND $<TARGET_FILE:vidl1_test_all> )

add_executable( vidl1_test_include test_include.cxx )
