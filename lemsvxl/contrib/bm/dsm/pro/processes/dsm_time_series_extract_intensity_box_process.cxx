//this is /contrib/bm/dsm/dsm_time_series_extract_intensity_box_process.cxx
#include<bprb/bprb_func_process.h>

#include"dsm/dsm_feature_sptr.h"
#include"dsm/dsm_pixel_time_series_map_sptr.h"

#include<vidl/vidl_convert.h>
#include<vidl/vidl_image_list_istream.h>

#include<vil/vil_convert.h>
#include<vil/vil_image_resource_sptr.h>
#include<vil/vil_image_view_base.h>

namespace dsm_time_series_extract_intensity_box_process_globals
{
	const unsigned int n_inputs_ = 6;
	const unsigned int n_outputs_ = 1;
}

bool dsm_time_series_extract_intensity_box_process_cons(bprb_func_process& pro)
{
	using namespace dsm_time_series_extract_intensity_box_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

	unsigned i = 0;
	input_types_[i++] = "vcl_string";//image glob
	input_types_[i++] = "unsigned"; //xmin
	input_types_[i++] = "unsigned"; //xmax
	input_types_[i++] = "unsigned"; //ymin
	input_types_[i++] = "unsigned"; //ymax
	input_types_[i++] = "bool";//normalized flag
	

	output_types_[0] = "dsm_pixel_time_series_map_sptr";

	if(!pro.set_input_types(input_types_))
		return false;

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}

bool dsm_time_series_extract_intensity_box_process(bprb_func_process& pro)
{
	using namespace dsm_time_series_extract_intensity_box_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << " dsm_time_series_extract_intensity_box_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	vcl_string img_glob = pro.get_input<vcl_string>(i++);
	unsigned xmin = pro.get_input<unsigned>(i++);
	unsigned xmax = pro.get_input<unsigned>(i++);
	unsigned ymin = pro.get_input<unsigned>(i++);
	unsigned ymax = pro.get_input<unsigned>(i++);
	bool normalize = pro.get_input<unsigned>(i++);
	
	vidl_image_list_istream video_stream(img_glob);

	unsigned nframes = video_stream.num_frames();

	if(nframes == 0)
	{
		vcl_cerr << "---- Error dsm_time_series_extract_intensity_box_process: The video stream is invalid. ----" << vcl_flush;
		return false;
	}

	dsm_pixel_time_series_map_sptr pixel_time_series_map_sptr = new dsm_pixel_time_series_map();

	for( unsigned frame = 0; frame < nframes; ++frame ) 
	{
		vcl_cout << "Processing Frame: " << frame << vcl_endl;
		vil_image_view<vxl_byte> curr_img;
		video_stream.seek_frame(frame);
		vidl_convert_to_view(*video_stream.current_frame(), curr_img);

		if( curr_img.nplanes() != 1 || curr_img.pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE )
		{
			vil_image_view<vxl_byte> grey_img;
			vil_convert_planes_to_grey(curr_img,grey_img);
			curr_img = grey_img;
		}//end grey image check

		for( unsigned x = xmin; x <= xmax; ++x )
			for( unsigned y = ymin; y <= ymax; ++y )
			{
#ifdef _DEBUG
				vcl_cout << "\tExtracting Intensity at pixel: (" << x << "," << y << "," << frame << ")" << vcl_endl;
#endif //_DEBUG
				dsm_feature_sptr feature_sptr;
				if( normalize )
					feature_sptr = new dsm_feature(vnl_vector<double>(1,curr_img(x,y,0)/vxl_byte(255)),frame);
				else
					feature_sptr = new dsm_feature(vnl_vector<double>(1,curr_img(x,y,0)),frame);

				pixel_time_series_map_sptr->insert(vgl_point_2d<unsigned>(x,y),frame,feature_sptr);
			}//end height iteration
	}//end frame iteration

	pro.set_output_val(0,pixel_time_series_map_sptr);

	return true;
}//end dsm_time_series_extract_intensity_box_process