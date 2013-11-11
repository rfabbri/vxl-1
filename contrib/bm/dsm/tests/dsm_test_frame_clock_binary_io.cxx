//this is /contrib/bm/dsm/tests/dsm_test_frame_clock_binary_io.cxx
#include"../dsm_frame_clock.h"
#include"../io/dsm_io_frame_clock.h"
#include<testlib/testlib_test.h>

#include<vbl/io/vbl_io_smart_ptr.h>

#include<vcl_string.h>

static void dsm_test_frame_clock_binary_io()
{
	vcl_string filename("fc_sptr.bin");

	dsm_frame_clock* fcp = dsm_frame_clock::instance();

	fcp->increment_time();
	fcp->increment_time();
	fcp->increment_time();

	vsl_b_ofstream os(filename.c_str(), vcl_ios::out|vcl_ios::binary);
	vsl_b_write(os,fcp);
	os.close();

	dsm_frame_clock* fcp2;
	//read the smart pointer
	vsl_b_ifstream is(filename.c_str(), vcl_ios::in|vcl_ios::binary);
	vsl_b_read(is,fcp2);
	is.close();

	TEST_NEAR("TEST FRAME CLOCK BINARY READ: ", fcp2->time(), unsigned(3), 1);

}//end test_dsm_frame_clock_binary_io

TESTMAIN(dsm_test_frame_clock_binary_io);