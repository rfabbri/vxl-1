//this is /contrib/bm/dsm/pro/processes/dsm_write_pixel_dat_process.cxx
#include<bprb/bprb_func_process.h>
#include<bprb/bprb_parameters.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_pixel_time_series_map_sptr.h>

#include<vcl_iomanip.h>
#include<vcl_sstream.h>

namespace dsm_write_pixel_dat_process_globals
{
	const unsigned int n_inputs_ = 4;
	const unsigned int n_ouputs_ = 0;
}

bool dsm_write_pixel_dat_process_cons( bprb_func_process& pro )
{
	using namespace dsm_write_pixel_dat_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);

	unsigned i = 0;
	input_types_[i++] = "dsm_pixel_time_series_map_sptr";
	input_types_[i++] = "unsigned";//x coordinate
	input_types_[i++] = "unsigned";//y coordinate
	input_types_[i++] = "vcl_string";//filename

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}

bool dsm_write_pixel_dat_process( bprb_func_process& pro )
{
	using namespace dsm_write_pixel_dat_process_globals;

	if( pro.n_inputs() != n_inputs_ )
	{
		vcl_cout << pro.name() << "dsm_write_pixel_dat_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	dsm_pixel_time_series_map_sptr pixel_time_series_map_sptr = pro.get_input<dsm_pixel_time_series_map_sptr>(i++);
	unsigned x = pro.get_input<unsigned>(i++);
	unsigned y = pro.get_input<unsigned>(i++);
	vcl_string filename = pro.get_input<vcl_string>(i++);

	dsm_pixel_time_series_map::pixel_time_series_map_type::const_iterator ts_itr, 
		ts_end = pixel_time_series_map_sptr->pixel_time_series_map.end();

	ts_itr = pixel_time_series_map_sptr->pixel_time_series_map.find(vgl_point_2d<unsigned>(x,y));

	if( ts_itr == ts_end )
	{
		vcl_cout << pro.name() << "dsm_write_pixel_dat_process:\n" 
				 <<"\tCould not find pixel: (" << x << "," << y <<") in pixel/time-series map." << vcl_endl;
		return false;
	}

	ts_itr->second->write_txt(filename);
	
	return true;
}