//this is /contrib/bm/vlfeat/tests/vlfeat_test_helloWorld.cxx
#include<testlib/testlib_test.h>

extern "C"
{
#include<vl/generic.h>
}

static void vlfeat_test_helloWorld()
{
	VL_PRINT("Hello World!\n");
}

TESTMAIN(vlfeat_test_helloWorld);