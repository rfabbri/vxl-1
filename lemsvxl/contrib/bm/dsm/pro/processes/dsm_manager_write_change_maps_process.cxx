//this is /contrib/bm/dsm/dsm_manager_write_change_maps_process.cxx
#include<bprb/bprb_func_process.h>
#include<brdb/brdb_value.h>

#include<dsm/dsm_manager_base_sptr.h>

#include<vcl_iomanip.h>
#include<vcl_sstream.h>

#include<vil/vil_image_view_base.h>
#include<vil/vil_convert.h>
#include<vil/vil_save.h>

#include<vidl/vidl_image_list_istream.h>
#include<vidl/vidl_convert.h>

#include<vul/vul_file.h>

namespace dsm_manager_write_change_maps_process_globals
{
	const unsigned int n_inputs_ = 3;
	const unsigned int n_outputs_ = 0;
}

//set input/output types
bool dsm_manager_write_change_maps_process_cons( bprb_func_process& pro )
{
	using namespace dsm_manager_write_change_maps_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);

	unsigned i = 0;
	input_types_[i++] = "dsm_manager_base_sptr";
	input_types_[i++] = "vcl_string";//the original image glob
	input_types_[i++] = "vcl_string";//output directory

	if(!pro.set_input_types(input_types_))
		return false;
	
	return true;
}

bool dsm_manager_write_change_maps_process( bprb_func_process& pro )
{
	using namespace dsm_manager_write_change_maps_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << " dsm_manager_write_change_maps_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	dsm_manager_base_sptr manager_sptr = pro.get_input<dsm_manager_base_sptr>(i++);
	vcl_string image_glob = pro.get_input<vcl_string>(i++);
	vcl_string result_directory = pro.get_input<vcl_string>(i++);

	vidl_image_list_istream video_stream(image_glob);

	unsigned nframes = video_stream.num_frames();

	vcl_map<vgl_point_2d<unsigned>, dsm_state_machine_base_sptr, dsm_vgl_point_2d_coord_compare<unsigned> >::const_iterator
		tsm_itr, tsm_end = manager_sptr->target_stateMachine_map_.end();

	vcl_stringstream map_directory;

	map_directory<< result_directory;

	if(!vul_file::is_directory(map_directory.str()))
		vul_file::make_directory(map_directory.str());

	for( unsigned frame = 0; frame < nframes; ++frame )
	{
		vcl_cout << "Writing Change Map: " << frame << " of " << nframes << vcl_endl;

		video_stream.seek_frame(frame);
		vil_image_view<vxl_byte> curr_img;
		vidl_convert_to_view(*video_stream.current_frame(), curr_img);
		
		vcl_stringstream map_filename;
		
		map_filename << map_directory.str() << "/change_map_" << vcl_setfill('0') << vcl_setw(4) << frame << ".png";
		

			
		for( tsm_itr = manager_sptr->target_stateMachine_map_.begin(); tsm_itr != tsm_end; ++tsm_itr )
		{
			//color image
			vxl_byte color;
			vgl_point_2d<unsigned> target = tsm_itr->first;
			bool change = tsm_itr->second->change(frame);
			if( change )
				color = 255;
			else
				color = 0;
			for(unsigned p = 0; p < curr_img.nplanes(); ++p)
			{
				curr_img(target.x(),target.y(),p) = color;
			}//end plane iteration
		}//end target/state machine iteration

		//save the image
		vil_save(curr_img,map_filename.str().c_str());

	}//end frame iteration
	
	return true;
}