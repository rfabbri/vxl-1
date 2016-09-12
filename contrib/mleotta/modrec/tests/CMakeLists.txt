# mleotta/modrec/tests/CMakeLists.txt
# Tests for modrec library

add_executable( modrec_test_all
  test_driver.cxx
  test_vehicle_state.cxx
)

target_link_libraries( modrec_test_all modrec testlib)

add_test( modrec_test_vehicle_state         modrec_test_all test_vehicle_state )

add_executable( modrec_test_include test_include.cxx )
target_link_libraries( modrec_test_include modrec )
