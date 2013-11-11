//this is /contrib/bm/dsm/pro/processes/dsm_time_series_sptr_from_map_process.cxx
#include<bprb/bprb_func_process.h>
#include<bprb/bprb_parameters.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_pixel_time_series_map_sptr.h>
//:NOTE THIS PROCESS WILL RETURN FALSE IF A TARGET PIXEL DOES NOT EXIST IN THE MAP

namespace dsm_time_series_sptr_from_map_process_globals
{
	const unsigned int n_inputs_ = 3;
	const unsigned int n_outputs_ = 1;
}

bool dsm_time_series_sptr_from_map_process_cons( bprb_func_process& pro )
{
	//set input/output types
	using namespace dsm_time_series_sptr_from_map_process_globals;
	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

	unsigned i = 0;
	input_types_[i++] = "dsm_pixel_time_series_map_sptr";//the map
	input_types_[i++] = "unsigned";//the x coordinate of the key pixel
	input_types_[i++] = "unsigned";//the y coordinate of the key pixel

	output_types_[0] = "dsm_time_series_sptr";//the output time series sptr

	if(!pro.set_input_types(input_types_))
		return false;

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}

bool dsm_time_series_sptr_from_map_process( bprb_func_process& pro )
{
	using namespace dsm_time_series_sptr_from_map_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << " dsm_time_series_sptr_from_map_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	dsm_pixel_time_series_map_sptr map_sptr = pro.get_input<dsm_pixel_time_series_map_sptr>(i++);
	unsigned key_x = pro.get_input<unsigned>(i++);
	unsigned key_y = pro.get_input<unsigned>(i++);

	dsm_pixel_time_series_map::pixel_time_series_map_type::const_iterator map_itr = map_sptr->time_series(key_x,key_y);

	if( map_itr == map_sptr->pixel_time_series_map.end() )
	{
		vcl_cout << pro.name() << " ERROR: dsm_time_series_sptr_from_map_process:\n"
			     << "\t the pixel at (" << key_x << "," << key_y << ") was not found in the pixel time series map." << vcl_endl;
		return false;
	}

	pro.set_output_val(0, map_itr->second);

	return true;
}