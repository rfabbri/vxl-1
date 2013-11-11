# contrib/Gregistration/georegister/tests/CMakeLists.txt
# Tests for dbsta library
INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/Gregistration )
ADD_EXECUTABLE( georegister_test_all
  test_driver.cxx
  test_dbrl_io.cxx
)

TARGET_LINK_LIBRARIES( georegister_test_all georegister testlib vpl vbl_io vul vsol vgl vidpro1)
ADD_TEST( test_georegister  georegister_test_all test_dbrl_io )


ADD_EXECUTABLE( georegister_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( georegister_test_include georegister )
