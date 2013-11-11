//this is /contrib/bm/dts/pro/processes/dts_pixel_time_series_write_pixel_txt_process.cxx
//:
// \file
// \date November 3, 2011
// \author Brandon A. Mayer
//
// Write a text file of the time series at a given pixel location
// given a dts_pixel_time_series_base_sptr
//
// Inputs: 
//         1. dts_pixel_time_series_base_sptr
//         2. output file (vcl_string)
//	       3. x coordinate (unsigned)
//         4. y coordinate (unsigned)
//
// Outputs:
//         None
//
// \verbatim
//  Modifications
// \endverbatim
#include<bprb/bprb_func_process.h>
#include<brdb/brdb_value.h>

#include<dts/dts_pixel_time_series.h>
#include<dts/dts_pixel_time_series_base_sptr.h>

namespace dts_pixel_time_series_write_txt_process_globals
{
	const unsigned int n_inputs_ = 4;
	const unsigned int n_outputs_ = 0;
}

//set input/outputs
bool dts_pixel_time_series_write_txt_process_cons( bprb_func_process& pro )
{
	using namespace dts_pixel_time_series_write_txt_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);

	unsigned i = 0;
	input_types_[i++] = "dts_pixel_time_series_base_sptr"; //from db
	input_types_[i++] = "vcl_string";//the output filename
	input_types_[i++] = "unsigned";//target x coordinate
	input_types_[i++] = "unsigned";//target y coordinate

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}

bool dts_pixel_time_series_write_txt_process( bprb_func_process& pro )
{
	using namespace dts_pixel_time_series_write_txt_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() 
            << " dts_pixel_time_series_write_txt_process: "
            << " The input number should be " 
            << n_inputs_ << "\n"
			<< "FILE: " << __FILE__ << '\n'
			<< "LINE: " << __LINE__ << '\n'
            << vcl_endl;
        return false;
	}

	//get inputs
	unsigned i = 0;
	dts_pixel_time_series_base_sptr dts_sptr = 
		pro.get_input<dts_pixel_time_series_base_sptr>(i++);

	vcl_string filename =
		pro.get_input<vcl_string>(i++);

	unsigned target_x = 
		pro.get_input<unsigned>(i++);

	unsigned target_y =
		pro.get_input<unsigned>(i++);

	//the target point
	vgl_point_2d<unsigned> target(target_x,target_y);

	return dts_sptr->write_txt(filename,target);
}