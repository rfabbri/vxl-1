# basic/dbul/tests/CMakeLists.txt
# Tests for dbul library

ADD_EXECUTABLE( dbul_test_all
  test_driver.cxx
  test_solar_position.cxx
  test_parse_simple_file.cxx
)

TARGET_LINK_LIBRARIES( dbul_test_all dbul vul testlib  )


ADD_TEST( dbul_test_solar_position         dbul_test_all test_solar_position )
ADD_TEST( dbul_test_parse_simple_file dbul_test_all test_parse_simple_file)

ADD_EXECUTABLE( dbul_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( dbul_test_include dbul )
