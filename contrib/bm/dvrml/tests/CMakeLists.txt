# this is /contrib/bm/dvrml/test/CMakeLists.txt
# Test for dvrml library

SET( dvrml_test_sources
	 test_driver.cxx
	 test_draw.cxx
#	 test_draw_point.cxx
#	 test_draw_box.cxx
#	 test_draw_axis.cxx
	 )

ADD_EXECUTABLE( dvrml_test_all ${dvrml_test_sources} )
TARGET_LINK_LIBRARIES( dvrml_test_all dvrml vgl testlib )

ADD_TEST( dvrml_test_draw_point		${EXECUTABLE_OUTPUT_PATH}/dvrml		test_draw_point )
ADD_TEST( dvrml_test_draw_box 		${EXECUTABLE_OUTPUT_PATH}/dvrml     test_draw_box )

#MESSAGE( "EXECUTABLE_OUTPUT_PATH = " ${EXECUTABLE_OUTPUT_PATH} )