//this is /contrib/bm/dsm/pro/processes/dsm_read_pixel_time_series_map_bin_process.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include"dsm/dsm_pixel_time_series_map_sptr.h"

namespace dsm_read_pixel_time_series_map_bin_process_globals
{
	const unsigned int n_inputs_ = 1;
	const unsigned int n_outputs_ = 1;
}

bool dsm_read_pixel_time_series_map_bin_process_cons(bprb_func_process& pro)
{
	using namespace dsm_read_pixel_time_series_map_bin_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

	input_types_[0] = "vcl_string";//the filename
	output_types_[0] = "dsm_pixel_time_series_map_sptr";//the output sptr

	if(!pro.set_input_types(input_types_))
		return false;

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}

bool dsm_read_pixel_time_series_map_bin_process(bprb_func_process& pro)
{
	using namespace dsm_read_pixel_time_series_map_bin_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << "dsm_write_pixel_time_series_map_bin_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get input
	unsigned i = 0;
	vcl_string filename = pro.get_input<vcl_string>(i++);

	vsl_b_ifstream is(filename.c_str(), vcl_ios::in|vcl_ios::binary);

	dsm_pixel_time_series_map_sptr p = new dsm_pixel_time_series_map();

	vsl_b_read(is,p);

	pro.set_output_val(0, p);

	return true;
}