//this is /contrib/bm/dsm/pro/processes/dsm_ncn_sptr_process.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_ncn_sptr.h>

//:global variables
namespace dsm_ncn_sptr_process_globals
{
	const unsigned int n_inputs_ = 0;
	const unsigned int n_outputs_ = 1;
}

//:sets input and output types
bool dsm_ncn_sptr_process_cons(bprb_func_process& pro)
{
	using namespace dsm_ncn_sptr_process_globals;

	vcl_vector<vcl_string> output_types_(n_outputs_);

	output_types_[0] = "dsm_ncn_sptr";

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}

bool dsm_ncn_sptr_process(bprb_func_process& pro)
{
	using namespace dsm_ncn_sptr_process_globals;

	dsm_ncn_sptr ncn_sptr = new dsm_ncn();

	pro.set_output_val(0,ncn_sptr);

	return true;
}