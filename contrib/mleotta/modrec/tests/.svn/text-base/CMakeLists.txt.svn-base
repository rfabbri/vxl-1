# mleotta/modrec/tests/CMakeLists.txt
# Tests for modrec library

ADD_EXECUTABLE( modrec_test_all
  test_driver.cxx
  test_vehicle_state.cxx
)

TARGET_LINK_LIBRARIES( modrec_test_all modrec testlib)

ADD_TEST( modrec_test_vehicle_state         modrec_test_all test_vehicle_state )

ADD_EXECUTABLE( modrec_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( modrec_test_include modrec )
