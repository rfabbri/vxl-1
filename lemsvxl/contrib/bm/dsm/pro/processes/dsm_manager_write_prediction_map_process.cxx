//this is /contrib/bm/dsm/dsm_manager_write_prediction_map_process.cxx
#include<bprb/bprb_func_process.h>

#include<dsm/dsm_manager_base_sptr.h>

#include<vcl_iostream.h>
#include<vcl_iomanip.h>
#include<vcl_sstream.h>

#include<vul/vul_file.h>

namespace dsm_manager_write_prediction_map_process_globals
{
	const unsigned int n_inputs_ = 5;
	const unsigned int n_outputs_ = 0;
}

bool dsm_manager_write_prediction_map_process_cons( bprb_func_process& pro )
{
	using namespace dsm_manager_write_prediction_map_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);

	unsigned i = 0;
	input_types_[i++] = "dsm_manager_base_sptr";
	input_types_[i++] = "unsigned";//(ni = image width)
	input_types_[i++] = "unsigned";//(nj = image height)
	input_types_[i++] = "vcl_string";//output directory
	input_types_[i++] = "vcl_string";//output format

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}

bool dsm_manager_write_prediction_map_process( bprb_func_process& pro )
{
	using namespace dsm_manager_write_prediction_map_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() 
			     << " dsm_manager_write_change_maps_process: The input number should be " 
				 << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	dsm_manager_base_sptr manager_sptr = pro.get_input<dsm_manager_base_sptr>(i++);
	unsigned ni = pro.get_input<unsigned>(i++);
	unsigned nj = pro.get_input<unsigned>(i++);
	vcl_string result_dir = pro.get_input<vcl_string>(i++);
	vcl_string format = pro.get_input<vcl_string>(i++);

	if(!vul_file::is_directory(result_dir))
		vul_file::make_directory(result_dir);

	vcl_map<vgl_point_2d<unsigned>, dsm_state_machine_base_sptr, dsm_vgl_point_2d_coord_compare<unsigned> >::const_iterator
		tsm_itr, tsm_end = manager_sptr->target_stateMachine_map_.end();

	unsigned nframes = manager_sptr->target_stateMachine_map_.begin()->second->frame_change_map_.size();

	format = "." + format;
	for( unsigned frame = 0; frame < nframes; ++frame )
	{
		vcl_stringstream filename;

		filename << result_dir << "/predicted_change_map_"  
				 << vcl_setfill('0') << vcl_setw(8) << frame << format;

	   /* if(format.compare("png") == 0)
			filename << ".png";
		else if(format.compare("jpg") == 0)
			filename << ".jpg";
		else
			filename << ".tiff";*/
		
		vcl_cout << "Writing Binary Prediction Change Map " << frame+1 << " of " << nframes;
		vcl_cout << "\t" << filename.str() << vcl_endl;

		vil_image_view<vxl_byte> change_view(ni,nj,1);

		change_view.fill(vxl_byte(127));

		for( tsm_itr = manager_sptr->target_stateMachine_map_.begin();
			 tsm_itr != tsm_end; ++tsm_itr )
		{
			vgl_point_2d<unsigned> target = tsm_itr->first;
			if( tsm_itr->second->change(frame) )
				change_view(target.x(), target.y(), 0) = 255;
			else
				change_view(target.x(), target.y(), 0) = 0;
				
		}//end target_stateMachine_map iteration

		vil_save(change_view,filename.str().c_str());
	}//end frame iteration

	return true;
}