# dvpgl/io/tests/CMakeLists.txt
# Tests for dvpgl_io library

ADD_EXECUTABLE( dvpgl_io_test_all
  # Driver source and utilities
  test_driver.cxx
  # The tests
#  test_misc.cxx
)

TARGET_LINK_LIBRARIES( dvpgl_io_test_all dvpgl_algo vsl vbl_io testlib )
#ADD_TEST( dvpgl_io_test_triangulation dvpgl_io_test_all test_triangulation)

ADD_EXECUTABLE( dvpgl_io_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( dvpgl_io_test_include dvpgl_io )
#ADD_EXECUTABLE( dvpgl_io_test_template_include test_template_include.cxx )
#TARGET_LINK_LIBRARIES( dvpgl_io_test_template_include dvpgl_io)
