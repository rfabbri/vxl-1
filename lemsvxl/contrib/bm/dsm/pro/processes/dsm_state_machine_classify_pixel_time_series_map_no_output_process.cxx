//this is /contrib/bm/dsm/pro/processes/dsm_state_machine_classify_pixel_time_series_map_no_output_process.cxx
#include<bprb/bprb_func_process.h>

#include<dsm/dsm_manager_base_sptr.h>
#include<dsm/dsm_manager.h>
#include<dsm/dsm_pixel_time_series_map_sptr.h>

#include<vul/vul_timer.h>

namespace dsm_state_machine_classify_pixel_time_series_map_no_output_process_globals
{
	const unsigned int n_inputs_ = 7;
	const unsigned int n_outputs_ = 1;
}

bool dsm_state_machine_classify_pixel_time_series_map_no_output_process_cons( bprb_func_process& pro )
{
	using namespace dsm_state_machine_classify_pixel_time_series_map_no_output_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

	unsigned i = 0;
	input_types_[i++] = "dsm_pixel_time_series_map_sptr";
	input_types_[i++] = "unsigned";//ndims
	input_types_[i++] = "unsigned";//t_forget
	input_types_[i++] = "double";//prob_thresh
	input_types_[i++] = "double";//mahalan_factor
	input_types_[i++] = "double";//init_covar
	input_types_[i++] = "double";//min_covar

	output_types_[0] = "dsm_manager_base_sptr";

    if(!pro.set_input_types(input_types_))
        return false;
    
	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}

bool dsm_state_machine_classify_pixel_time_series_map_no_output_process( bprb_func_process& pro )
{
	using namespace dsm_state_machine_classify_pixel_time_series_map_no_output_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << " dsm_state_machine_classify_pixel_time_series_map_no_output_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	dsm_pixel_time_series_map_sptr pixel_time_series_map_sptr = pro.get_input<dsm_pixel_time_series_map_sptr>(i++);
	unsigned ndims = pro.get_input<unsigned>(i++);
	unsigned t_forget = pro.get_input<unsigned>(i++);
	double prob_thresh = pro.get_input<double>(i++);
	double mahalan_factor = pro.get_input<double>(i++);
	double dinit_covar = pro.get_input<double>(i++);
	double dmin_covar = pro.get_input<double>(i++);

	if( pixel_time_series_map_sptr->pixel_time_series_map.empty() )
	{
		vcl_cerr << "---- ERROR: dsm_state_machine_classify_pixel_time_series_map, the pixel time series map is empty. ----" << vcl_endl;
		return false;
	}

	dsm_manager_base_sptr manager_base_sptr;

	
	switch(ndims)
	{
	case 1:
		{
		vcl_cout << "Using a 1d State Machine." << vcl_endl;
			vnl_matrix<double> init_covar = vnl_diag_matrix<double>(ndims, dinit_covar);
			vnl_matrix<double> min_covar = vnl_diag_matrix<double>(ndims, dmin_covar);

			manager_base_sptr = new dsm_manager<1>();

			dsm_pixel_time_series_map::pixel_time_series_map_type::const_iterator 
				t_itr, t_end = pixel_time_series_map_sptr->pixel_time_series_map.end();
			unsigned state_machine_id;

			unsigned num_targets = pixel_time_series_map_sptr->pixel_time_series_map.size();

			unsigned nframes = pixel_time_series_map_sptr->pixel_time_series_map.begin()->second->time_series.size();

			vul_timer timer;

			timer.mark();


			for( unsigned frame = 0; frame < nframes; ++frame )
			{
				for( state_machine_id = 0, t_itr = pixel_time_series_map_sptr->pixel_time_series_map.begin(); t_itr != t_end; ++t_itr, ++state_machine_id )
				{
					dsm_state_machine_base_sptr state_machine_base_sptr;
					if(frame == 0)
					{
						vcl_cout << "Creating State Machine " << state_machine_id + 1 << " of " << num_targets <<  " for target: (" << t_itr->first.x() << "," << t_itr->first.y() << ")" << vcl_endl;
						state_machine_base_sptr = new dsm_state_machine<1>( state_machine_id,
							t_forget, prob_thresh,
							mahalan_factor,
							init_covar, min_covar);
						manager_base_sptr->insert_state_machine(t_itr->first,state_machine_base_sptr);
					}//end if(frame == 0)
					else
						state_machine_base_sptr = manager_base_sptr->state_machine(t_itr->first);

					vcl_cout << "Classifying Frame: " << frame << " of " << nframes << '\n'
						     <<"\tState Machine: " << state_machine_id + 1  << " of " << num_targets << vcl_endl;
					
					state_machine_base_sptr->classify(t_itr->second->time_series[frame]);

					
				}//end pixel/time-series map iteration

				manager_base_sptr->frame_clock_ptr_->increment_time();
			}//end frame iteration
			
			vcl_cout << "Classified " << num_targets << " in " << timer.real() << " milliseconds." << vcl_endl;
		}//end case 1
	case 2:
		{
			vcl_cout << "Using a 2d State Machine." << vcl_endl;
			vnl_matrix<double> init_covar = vnl_diag_matrix<double>(ndims, dinit_covar);
			vnl_matrix<double> min_covar = vnl_diag_matrix<double>(ndims, dmin_covar);

			manager_base_sptr = new dsm_manager<2>();

			dsm_pixel_time_series_map::pixel_time_series_map_type::const_iterator 
				t_itr, t_end = pixel_time_series_map_sptr->pixel_time_series_map.end();
			unsigned state_machine_id;

			unsigned num_targets = pixel_time_series_map_sptr->pixel_time_series_map.size();

			unsigned nframes = pixel_time_series_map_sptr->pixel_time_series_map.begin()->second->time_series.size();

			vul_timer timer;

			timer.mark();


			for( unsigned frame = 0; frame < nframes; ++frame )
			{
				for( state_machine_id = 0, t_itr = pixel_time_series_map_sptr->pixel_time_series_map.begin(); t_itr != t_end; ++t_itr, ++state_machine_id )
				{
					dsm_state_machine_base_sptr state_machine_base_sptr;
					if(frame == 0)
					{
						vcl_cout << "Creating State Machine " << state_machine_id + 1 << " of " << num_targets <<  " for target: (" << t_itr->first.x() << "," << t_itr->first.y() << ")" << vcl_endl;
						state_machine_base_sptr = new dsm_state_machine<2>( state_machine_id,
							t_forget, prob_thresh,
							mahalan_factor,
							init_covar, min_covar);
						manager_base_sptr->insert_state_machine(t_itr->first,state_machine_base_sptr);
					}//end if(frame == 0)
					else
						state_machine_base_sptr = manager_base_sptr->state_machine(t_itr->first);

					vcl_cout << "Classifying Frame: " << frame << " of " << nframes << '\n'
						     <<"\tState Machine: " << state_machine_id + 1  << " of " << num_targets << vcl_endl;
					
					state_machine_base_sptr->classify(t_itr->second->time_series[frame]);

					
				}//end pixel/time-series map iteration

				manager_base_sptr->frame_clock_ptr_->increment_time();
			}//end frame iteration
			
			vcl_cout << "Classified " << num_targets << " in " << timer.real() << " milliseconds." << vcl_endl;

		}//end case 2
		break;
	default:
		{
			vcl_cerr << "---- ERROR PROCESS DSM_STATE_MACHINE_CLASSIFY_PIXEL_TIME_SERIES_MAP_NO_OUTPUT_PROCESS:\n"
				     << "     no implmentation for ndims = " << ndims << '\n' << vcl_flush;
			return false;
		}//end default
	}//end switch

	pro.set_output_val(0,manager_base_sptr);

	return true;
}