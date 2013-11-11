//this is /contrib/bm/vlfeat/tests/test_driver.cxx
#include<testlib\testlib_register.h>

DECLARE( vlfeat_test_helloWorld );
DECLARE( vlfeat_test_dsift );

void register_tests()
{
	REGISTER( vlfeat_test_helloWorld );
	REGISTER( vlfeat_test_dsift );
}

DEFINE_MAIN;