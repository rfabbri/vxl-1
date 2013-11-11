//this is /contrib/bm/dsm/pro/process/dsm_target_neighborhood_map_from_ncn_sptr_process.cxx
#include<bprb/bprb_func_process.h>
#include<bprb/bprb_parameters.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_ncn_sptr.h>
#include<dsm/dsm_target_neighborhood_map_sptr.h>

//: Process to extract a target_neighborhood_map_sptr form an ncn_sptr class

namespace dsm_target_neighborhood_map_from_ncn_sptr_process_globals
{
	const unsigned int n_inputs_ = 1;
	const unsigned int n_outputs_ = 1;
}

bool dsm_target_neighborhood_map_from_ncn_sptr_process_cons(bprb_func_process& pro)
{
	//set input/output types
	using namespace dsm_target_neighborhood_map_from_ncn_sptr_process_globals;
	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);
	input_types_[0] = "dsm_ncn_sptr";
	output_types_[0] = "dsm_target_neighborhood_map_sptr";

	if(!pro.set_input_types(input_types_))
		return false;

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}

//the process
bool dsm_target_neighborhood_map_from_ncn_sptr_process(bprb_func_process& pro)
{
	using namespace dsm_target_neighborhood_map_from_ncn_sptr_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << "dsm_target_neighborhood_map_from_ncn_sptr_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get input
	dsm_ncn_sptr ncn_sptr = pro.get_input<dsm_ncn_sptr>(0);

	dsm_target_neighborhood_map_sptr target_neighborhood_map_sptr = new dsm_target_neighborhood_map;

	target_neighborhood_map_sptr->target_neighborhood_map_ = ncn_sptr->neighborhood();

	//set output
	pro.set_output_val(0, target_neighborhood_map_sptr);
	return true;
}