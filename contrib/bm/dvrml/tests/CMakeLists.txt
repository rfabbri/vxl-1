# this is /contrib/bm/dvrml/test/CMakeLists.txt
# Test for dvrml library

set( dvrml_test_sources
	 test_driver.cxx
	 test_draw.cxx
#	 test_draw_point.cxx
#	 test_draw_box.cxx
#	 test_draw_axis.cxx
	 )

add_executable( dvrml_test_all ${dvrml_test_sources} )
target_link_libraries( dvrml_test_all dvrml vgl testlib )

add_test( dvrml_test_draw_point		${EXECUTABLE_OUTPUT_PATH}/dvrml		test_draw_point )
add_test( dvrml_test_draw_box 		${EXECUTABLE_OUTPUT_PATH}/dvrml     test_draw_box )

#message( "EXECUTABLE_OUTPUT_PATH = " ${EXECUTABLE_OUTPUT_PATH} )