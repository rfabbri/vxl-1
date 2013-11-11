//this is /contrib/bm/dsm/rpo/processes/dsm_pixel_time_series_map_sptr_extract_sift_and_reduce_process.cxx
#include<bapl/bapl_dsift_sptr.h>

#include<bprb/bprb_func_process.h>
#include<brdb/brdb_value.h>

#include"dsm/dsm_feature_sptr.h"
#include"dsm/dsm_pixel_time_series_map_sptr.h"

#include<vidl/vidl_convert.h>
#include<vidl/vidl_image_list_istream.h>

#include<vil/vil_convert.h>
#include<vil/vil_image_resource_sptr.h>
#include<vil/vil_image_view_base.h>

namespace dsm_pixel_time_series_map_sptr_extract_sift_and_reduce_process_globals
{
	const unsigned int n_inputs_ = 6;
	const unsigned int n_outputs_ = 1;
}

bool dsm_pixel_time_series_map_sptr_extract_sift_and_reduce_process_cons( bprb_func_process& pro )
{
	using namespace dsm_pixel_time_series_map_sptr_extract_sift_and_reduce_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

	unsigned i = 0;
	input_types_[i++] = "vcl_string";//the image glob
	input_types_[i++] = "unsigned";//xmin
	input_types_[i++] = "unsigned";//xmax
	input_types_[i++] = "unsigned";//ymin
	input_types_[i++] = "unsigned";//ymax
	input_types_[i++] = "unsigned";//ndims2keep

	output_types_[0] = "dsm_pixel_time_series_map_sptr";

	if(!pro.set_input_types(input_types_))
		return false;

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}

bool dsm_pixel_time_series_map_sptr_extract_sift_and_reduce_process( bprb_func_process& pro )
{
	using namespace dsm_pixel_time_series_map_sptr_extract_sift_and_reduce_process_globals;
	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << " dsm_pixel_time_series_map_sptr_extract_sift_and_reduce_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get intputs
	unsigned i = 0;
	vcl_string img_glob = pro.get_input<vcl_string>(i++);
	unsigned xmin		= pro.get_input<unsigned>(i++);
	unsigned xmax		= pro.get_input<unsigned>(i++);
	unsigned ymin		= pro.get_input<unsigned>(i++);
	unsigned ymax		= pro.get_input<unsigned>(i++);
	unsigned ndims2keep = pro.get_input<unsigned>(i++);

	vidl_image_list_istream video_stream(img_glob);
	video_stream.seek_frame(0);
	unsigned nframes=video_stream.num_frames();

	if(!video_stream.is_valid() || nframes < 1)
	{
		vcl_cout << pro.name() << " dsm_pixel_time_series_map_sptr_extract_sift_and_reduce_process: The video stream is not valid." << vcl_endl;
		return false;
	}

	

	dsm_pixel_time_series_map_sptr pixel_time_series_map_sptr = new dsm_pixel_time_series_map(128);

	for( unsigned t = 0; t < nframes; ++t )
	{
		vcl_cout << "Processing Frame: " << t << vcl_endl;

		video_stream.seek_frame(t);

		vil_image_view<vxl_byte> curr_img;
		
		vidl_convert_to_view(*video_stream.current_frame(), curr_img);

		bapl_dsift dsift;

		if( curr_img.nplanes() != 1 || curr_img.pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE )
		{
			vil_image_view<vxl_byte> grey_img;
			vil_convert_planes_to_grey(curr_img,grey_img);
			dsift.set_img(grey_img);
		}
		else
			dsift.set_img(curr_img);

		for(unsigned x = xmin; x <= xmax; ++x)
			for(unsigned y = ymin; y <= ymax; ++y)
			{
				vcl_cout << "\tExtracting Sift at pixel: (" << x << "," << y << ")" << vcl_endl;
				dsm_feature_sptr feature_sptr = new dsm_feature(dsift.vnl_dsift(x,y),t);
				pixel_time_series_map_sptr->insert(vgl_point_2d<unsigned>(x,y),t,feature_sptr);
			}

	}//end frame iteration

	pixel_time_series_map_sptr->reduce_dims(ndims2keep);

	pro.set_output_val(0,pixel_time_series_map_sptr);

	return true;
}