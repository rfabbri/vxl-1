# This is contrib/bm/dbcl/tests/CMakeLists.txt
# Tests for dbcl library

SET( dbcl_test_sources
	 test_driver.cxx
	 test_linear_classifier.cxx 	 
	 )
	 
ADD_EXECUTABLE( dbcl_test_all ${dbcl_test_sources} )
TARGET_LINK_LIBRARIES( dbcl_test_all dbcl vgl vnl testlib ) 

ADD_TEST( dbcl_test_linear_classifier				${EXECUTABLE_OUTPUT_PATH}/dbcl		test_linear_classifier )

