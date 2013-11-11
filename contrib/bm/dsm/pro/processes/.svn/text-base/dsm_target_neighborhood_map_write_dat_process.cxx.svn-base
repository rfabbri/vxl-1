//this is /contrib/bm/dsm/pro/processes/dsm_target_neighborhood_map_write_dat_process.cxx
#include<bprb/bprb_func_process.h>
#include<bprb/bprb_parameters.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_target_neighborhood_map_sptr.h>

namespace dsm_target_neighborhood_map_write_dat_process_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 0;
}

bool dsm_target_neighborhood_map_write_dat_process_cons(bprb_func_process& pro)
{
	//set input/output types
	using namespace dsm_target_neighborhood_map_write_dat_process_globals;
	vcl_vector<vcl_string> input_types_(n_inputs_);
	unsigned i = 0;
	input_types_[i++] = "vcl_string";//directory to write to
	input_types_[i++] = "dsm_target_neighborhood_map_sptr";//the sptr to write

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}

bool dsm_target_neighborhood_map_write_dat_process(bprb_func_process& pro)
{
	using namespace dsm_target_neighborhood_map_write_dat_process_globals;

	if( pro.n_inputs() != n_inputs_ )
	{
		vcl_cout << pro.name() << "dsm_target_neighborhood_map_write_dat_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}	

	//get inputs
	unsigned i = 0;
	vcl_string filename = pro.get_input<vcl_string>(i++);
	dsm_target_neighborhood_map_sptr target_neighborhood_map = pro.get_input<dsm_target_neighborhood_map_sptr>(i++);

	return target_neighborhood_map->write_txt(filename);
}