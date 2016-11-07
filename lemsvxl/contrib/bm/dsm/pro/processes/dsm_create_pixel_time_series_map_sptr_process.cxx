//this is /contrib/bm/dsm/pro/processes/dsm_create_pixel_time_series_map_sptr_process.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_pixel_time_series_map_sptr.h>

#include<vcl_vector.h>

namespace dsm_create_pixel_time_series_map_sptr_process_globals
{
	const unsigned int n_inputs_ = 1;
	const unsigned int n_outputs_ = 1;
}

bool dsm_create_pixel_time_series_map_sptr_process_cons(bprb_func_process& pro)
{
	//set input/output types
	using namespace dsm_create_pixel_time_series_map_sptr_process_globals;
	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);
	input_types_[0] = "unsigned";//the feature dimension
	output_types_[0] = "dsm_pixel_time_series_map_sptr";

	if(!pro.set_input_types(input_types_))
		return false;

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}

bool dsm_create_pixel_time_series_map_sptr_process(bprb_func_process& pro)
{
	using namespace dsm_create_pixel_time_series_map_sptr_process_globals;
	//get input
	unsigned ndims = pro.get_input<unsigned>(0);
	
	dsm_pixel_time_series_map_sptr tsm_sptr = new dsm_pixel_time_series_map(ndims);

	pro.set_output_val(0,tsm_sptr);
	return true;
}