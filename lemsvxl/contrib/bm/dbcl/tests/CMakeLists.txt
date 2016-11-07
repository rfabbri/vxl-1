# This is contrib/bm/dbcl/tests/CMakeLists.txt
# Tests for dbcl library

set( dbcl_test_sources
	 test_driver.cxx
	 test_linear_classifier.cxx 	 
	 )
	 
add_executable( dbcl_test_all ${dbcl_test_sources} )
target_link_libraries( dbcl_test_all dbcl vgl vnl testlib ) 

add_test( dbcl_test_linear_classifier				${EXECUTABLE_OUTPUT_PATH}/dbcl		test_linear_classifier )

