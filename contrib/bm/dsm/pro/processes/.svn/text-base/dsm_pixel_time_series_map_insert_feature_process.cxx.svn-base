//this is /contrib/bm/dsm/pro/processes/dsm_pixel_time_series_map_insert_feature_process.cxx
#include<bprb/bprb_func_process.h>
#include<bprb/bprb_parameters.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_pixel_time_series_map_sptr.h>

//: NOTE IF THE TARGET PIXEL IS NOT FOUND IN THE MAP THE CORRESPONDING KEY AND TIME SERIES VALUE WILL BE ADDED.

namespace dsm_pixel_time_series_map_insert_feature_process_globals
{
	const unsigned int n_inputs_ = 5;
	const unsigned int n_outputs_ = 0;
}

bool dsm_pixel_time_series_map_insert_feature_process_cons( bprb_func_process& pro )
{
	//set input/output types
	using namespace dsm_pixel_time_series_map_insert_feature_process_globals;
	vcl_vector<vcl_string> input_types_(n_inputs_);
	unsigned i = 0;
	input_types_[i++] = "dsm_pixel_time_series_map_sptr";//the map sptr
	input_types_[i++] = "unsigned";//x coordinate of the key pixel
	input_types_[i++] = "unsigned";//y coordinate of the key pixel
	input_types_[i++] = "unsigned";//the time
	input_types_[i++] = "dsm_feature_sptr";//the feature sptr

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}

bool dsm_pixel_time_series_map_insert_feature_process( bprb_func_process& pro )
{
	using namespace dsm_pixel_time_series_map_insert_feature_process_globals;

	if( pro.n_inputs() != n_inputs_ )
	{
		vcl_cout << pro.name() << "dsm_pixel_time_series_map_insert_feature_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	dsm_pixel_time_series_map_sptr map_sptr = pro.get_input<dsm_pixel_time_series_map_sptr>(i++);
	unsigned key_x = pro.get_input<unsigned>(i++);
	unsigned key_y = pro.get_input<unsigned>(i++);
	unsigned time = pro.get_input<unsigned>(i++);
	dsm_feature_sptr feature_sptr = pro.get_input<dsm_feature_sptr>(i++);

	map_sptr->insert(key_x,key_y,time,feature_sptr);

	return true;
}