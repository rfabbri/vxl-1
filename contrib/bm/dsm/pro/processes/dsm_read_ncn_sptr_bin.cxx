//this is /contrib/bm/dsm/pro/processes/dsm_read_ncn_sptr_bin.cxx
//:
// \file
// \date January 30, 2011
// \author Brandon A. Mayer
//
// Process to read a dsm_ncn_sptr from disk and put into python database.
//
// Inputs: 1. Filename
// Ouput:  1. dsm_ncn_sptr
//
// \verbatim
//  Modifications
// \endverbatim

#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_ncn_sptr.h>

#include<vcl_iostream.h>
#include<vcl_vector.h>
#include<vcl_string.h>

#include<vbl/io/vbl_io_smart_ptr.h>

//:global variables
namespace dsm_read_ncn_sptr_bin_globals
{
	const unsigned int n_inputs_ = 1;
	const unsigned int n_outputs_ = 1;
}

bool dsm_read_ncn_sptr_bin_process_cons(bprb_func_process& pro)
{
	//set input/output types
	using namespace dsm_read_ncn_sptr_bin_globals;
	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

	input_types_[0] = "vcl_string";
	output_types_[0] = "dsm_ncn_sptr";

	if(!pro.set_input_types(input_types_))
		return false;

	if(!pro.set_output_types(output_types_))
		return false;

	return true;

}//end dsm_read_ncn_sptr_bin_process_cons

bool dsm_read_ncn_sptr_bin_process(bprb_func_process& pro)
{
	using namespace dsm_read_ncn_sptr_bin_globals;

	unsigned i = 0;
	vcl_string filename = pro.get_input<vcl_string>(i++);	

	vsl_b_ifstream is(filename.c_str(), vcl_ios::in|vcl_ios::binary);
	dsm_ncn_sptr ncn_sptr = new dsm_ncn();

	vsl_b_read(is, ncn_sptr);

	pro.set_output_val(0,ncn_sptr);

	return true;
}//end dsm_read_ncn_sptr_bin_process