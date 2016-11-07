//this is /contrib/bm/dsm/pro/processes/dsm_state_machine.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_state_machine.h>

#include<vcl_iostream.h>
#include<vcl_vector.h>
#include<vcl_string.h>

namespace dsm_update_state_machine_process_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 0;
}

bool dsm_update_state_machine_process_cons(bprb_func_process& pro)
{
	using namespace dsm_update_state_machine_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

    input_types_[0] = "unsigned";

	output_types_[0] = "dsm_features_sptr";

    if(!pro.set_input_types(input_types_))
        return false;
    
	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}//end dsm_state_machine_process_cons

bool dsm_update_state_machine_process(bprb_func_process &pro)
{
	return true;
}//end dsm_update_state_machine_process