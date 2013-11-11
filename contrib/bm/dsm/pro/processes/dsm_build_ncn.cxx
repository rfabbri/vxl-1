// \date January 30, 2011
// \author Brandon A. Mayer
//
// Process to build a non compact nieghborhood
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

//: global variables
namespace dsm_build_ncn_globals
{
	const unsigned int n_inputs_ = 1;
	const unsigned int n_outputs_ = 0;
}//build dsm_build_ncn_globals

bool dsm_build_ncn_process_cons( bprb_func_process& pro )
{
	//set input/output types
	using namespace dsm_build_ncn_globals;
	vcl_vector<vcl_string> input_types_(n_inputs_);

	input_types_[0] = "dsm_ncn_sptr";

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}//end dsm_build_ncn_process_cons

bool dsm_build_ncn_process(bprb_func_process& pro)
{
	using namespace dsm_build_ncn_globals;

	unsigned i = 0;
	dsm_ncn_sptr ncn_sptr = pro.get_input<dsm_ncn_sptr>(i++);

	return ncn_sptr->build_ncn();

}//end dsm_build_ncn_process