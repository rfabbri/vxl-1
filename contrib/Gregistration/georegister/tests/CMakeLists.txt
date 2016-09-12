# contrib/Gregistration/georegister/tests/CMakeLists.txt
# Tests for dbsta library
include_directories( ${lems_SOURCE_DIR}/contrib/Gregistration )
add_executable( georegister_test_all
  test_driver.cxx
  test_dbrl_io.cxx
)

target_link_libraries( georegister_test_all georegister testlib vpl vbl_io vul vsol vgl vidpro1)
add_test( test_georegister  georegister_test_all test_dbrl_io )


add_executable( georegister_test_include test_include.cxx )
target_link_libraries( georegister_test_include georegister )
