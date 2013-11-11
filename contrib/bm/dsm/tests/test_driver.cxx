//this is /contrib/bm/dsm/tests/test_driver.cxx
//:
// \file
// \date March 11, 2010
// \author Brandon A. Mayer
//
// This is the test driver for dsm test programs.
//
// \verbatim
//  Modifications
// \endverbatim
#include<testlib/testlib_register.h>

DECLARE( dsm_test_frame_clock_binary_io );
DECLARE( dsm_test_node_binary_io );
DECLARE( dsm_test_state_machine_binary_io );
DECLARE( dsm_test_map_binary_io );
DECLARE( dsm_test_manager );


void register_tests()
{
	REGISTER( dsm_test_frame_clock_binary_io );
	REGISTER( dsm_test_node_binary_io);
	REGISTER( dsm_test_state_machine_binary_io );
    REGISTER( dsm_test_map_binary_io );
    REGISTER( dsm_test_manager );
}

DEFINE_MAIN;