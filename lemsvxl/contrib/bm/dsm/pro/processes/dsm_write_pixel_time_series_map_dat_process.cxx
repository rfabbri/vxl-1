//this is /contrib/bm/dsm/pro/processes/dsm_write_pixel_time_series_map_dat_process.cxx
#include<bprb/bprb_func_process.h>
#include<bprb/bprb_parameters.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_pixel_time_series_map_sptr.h>

namespace dsm_write_pixel_time_series_map_dat_process_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 0;
}

bool dsm_write_pixel_time_series_map_dat_process_cons( bprb_func_process& pro )
{
	//set input/output types
	using namespace dsm_write_pixel_time_series_map_dat_process_globals;
	vcl_vector<vcl_string> input_types_(n_inputs_);
	unsigned i = 0;
	input_types_[i++] = "vcl_string";//the directory to write to
	input_types_[i++] = "dsm_pixel_time_series_map_sptr";//the sptr to write

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}

bool dsm_write_pixel_time_series_map_dat_process( bprb_func_process& pro )
{
	using namespace dsm_write_pixel_time_series_map_dat_process_globals;

	if( pro.n_inputs() != n_inputs_ )
	{
		vcl_cout << pro.name() << "dsm_write_time_series_dat_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	vcl_string filename = pro.get_input<vcl_string>(i++);
	dsm_pixel_time_series_map_sptr p = pro.get_input<dsm_pixel_time_series_map_sptr>(i++);

	p->write_txt(filename);

	return true;

}