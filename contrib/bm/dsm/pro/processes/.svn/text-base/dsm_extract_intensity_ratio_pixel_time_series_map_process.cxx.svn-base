//this is /contrib/bm/dsm/pro/processes/dsm_extract_intensity_ratio_pixel_time_series_map_process.cxx
#include<bprb/bprb_func_process.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_target_neighborhood_map_sptr.h>
#include<dsm/dsm_feature_sptr.h>
#include<dsm/dsm_pixel_time_series_map_sptr.h>

#include<vidl/vidl_image_list_istream.h>
#include<vidl/vidl_convert.h>

#include<vil/vil_image_view.h>
#include<vil/vil_convert.h>

namespace dsm_extract_intensity_ratio_pixel_time_series_map_process_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 1;
}

bool dsm_extract_intensity_ratio_pixel_time_series_map_process_cons(bprb_func_process& pro)
{
	using namespace dsm_extract_intensity_ratio_pixel_time_series_map_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

	unsigned i = 0;
	input_types_[i++] = "dsm_target_neighborhood_map_sptr";//the target/neighborhood map
	input_types_[i++] = "vcl_string";//video glob

	output_types_[0] = "dsm_pixel_time_series_map_sptr";

	if(!pro.set_input_types(input_types_))
		return false;

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}

bool dsm_extract_intensity_ratio_pixel_time_series_map_process(bprb_func_process& pro)
{
	using namespace dsm_extract_intensity_ratio_pixel_time_series_map_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << " dsm_extract_intensity_ratio_pixel_time_series_map_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	dsm_target_neighborhood_map_sptr target_neighborhood_map_sptr = pro.get_input<dsm_target_neighborhood_map_sptr>(i++);
	vcl_string video_glob = pro.get_input<vcl_string>(i++);
	
	unsigned num_neighbors = target_neighborhood_map_sptr->num_neighbors();

	vidl_image_list_istream video_stream(video_glob);

	unsigned nframes = video_stream.num_frames();

	dsm_pixel_time_series_map_sptr pixel_time_series_map_sptr = new dsm_pixel_time_series_map(num_neighbors);

	for(unsigned t = 0; t < nframes; ++t)
	{
		vcl_cout << "Processing frame " << t << " of " << nframes << vcl_endl;

		vil_image_view<vxl_byte> grey_img, curr_img;
		video_stream.seek_frame(t);
		vidl_convert_to_view(*video_stream.current_frame(), curr_img);
		if(curr_img.nplanes() != 1 || curr_img.pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE)
		{
			vil_convert_planes_to_grey(curr_img, grey_img);
		}
		else
			grey_img = curr_img;

		dsm_target_neighborhood_map::dsm_target_neighborhood_map_type::const_iterator 
			t_itr, t_end = target_neighborhood_map_sptr->target_neighborhood_map_.end();

		unsigned t_idx;
		for( t_idx = 0, t_itr=target_neighborhood_map_sptr->target_neighborhood_map_.begin(); t_itr != t_end; ++t_itr, ++t_idx)
		{
			vcl_cout << "\t Processing target " << t_idx << " of " << target_neighborhood_map_sptr->target_neighborhood_map_.size() << vcl_endl;

			vnl_vector<double> intensity_ratio(t_itr->second.size());

			double target_intensity = grey_img(t_itr->first.x(),t_itr->first.y());

			vcl_vector<vgl_point_2d<unsigned> >::const_iterator n_itr, n_end = t_itr->second.end();

			unsigned indx;
			for( indx = 0, n_itr = t_itr->second.begin(); n_itr != n_end; ++n_itr, ++indx )
			{
				vcl_cout << "\t\t Processing neighbor " << indx << " of " << t_itr->second.size() << vcl_endl;
				intensity_ratio[indx] = target_intensity/grey_img(n_itr->x(), n_itr->y());
			}//end neighborhood iteration

			dsm_feature_sptr feature_sptr = new dsm_feature(intensity_ratio,t);
			pixel_time_series_map_sptr->insert(t_itr->first,t,feature_sptr);
		}//end target iteration
	}//end frame iteration

	video_stream.close();

	pro.set_output_val(0,pixel_time_series_map_sptr);

	return true;
}