//this is /contrib/bm/dsm/pro/processes/dsm_manager_write_average_num_nodes_per_frame_process.cxx
#include<bprb/bprb_func_process.h>

#include<dsm/dsm_manager_base_sptr.h>

#include<iterator>//for distance function
#include<iostream>
#include<iomanip>

namespace dsm_manager_write_average_num_states_per_frame_process_globals
{
	const unsigned int n_inputs_ = 2;
	const unsigned int n_outputs_ = 0;
}

bool dsm_manager_write_average_num_states_per_frame_process_cons( bprb_func_process& pro )
{
	using namespace dsm_manager_write_average_num_states_per_frame_process_globals;

	std::vector<std::string> input_types_(n_inputs_);

	unsigned i = 0;
	input_types_[i++] = "dsm_manager_base_sptr";
	input_types_[i++] = vcl_string";//output filename

	if(!pro.set_input_types(input_types_))
		return false;

	return true;
}

bool dsm_manager_write_average_num_states_per_frame_process( bprb_func_process& pro )
{
	using namespace dsm_manager_write_average_num_states_per_frame_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		std::cout << pro.name() << " dsm_manager_write_average_num_nodes_per_frame_process: The input number should be " << n_inputs_ << std::endl;
		return false;
	}
	
	//get inputs
	unsigned i = 0;
	dsm_manager_base_sptr manager_base_sptr = pro.get_input<dsm_manager_base_sptr>(i++);
	std::string filename = pro.get_input<std::string>(i++);

	std::map<vgl_point_2d<unsigned>, dsm_state_machine_base_sptr, dsm_vgl_point_2d_coord_compare<unsigned> >::const_iterator
		target_state_machine_itr, target_state_machine_end = manager_base_sptr->target_stateMachine_map_.end();

	//this vector is the length of the number of frames
	//each element is the the aggregate number of graphs at a given frame.
	std::vector<double> graph_size_counts(1,0.0);

	for(target_state_machine_itr = manager_base_sptr->target_stateMachine_map_.begin();
			target_state_machine_itr != target_state_machine_end; ++target_state_machine_itr )
	{
		dsm_state_machine_base_sptr state_machine_base_sptr = target_state_machine_itr->second;

		std::map<unsigned, std::map<unsigned, vnl_vector<double> > >::const_iterator
			frame_mean_map_itr, frame_mean_map_end = state_machine_base_sptr->frame_mean_map_.end();


		for( frame_mean_map_itr = state_machine_base_sptr->frame_mean_map_.begin();
				frame_mean_map_itr != frame_mean_map_end; ++frame_mean_map_itr )
		{
			unsigned indx = state_machine_base_sptr->frame_mean_map_.size() - std::distance(frame_mean_map_itr,frame_mean_map_end);
			if( graph_size_counts.size() <= indx )
				graph_size_counts.push_back(0.0);
			graph_size_counts[indx]+=frame_mean_map_itr->second.size();

		}//end frame/mean iteration

	}//end target/state-machine iteration

	std::ofstream of(filename.c_str(), std::ios::out);
	double nstate_machines = manager_base_sptr->target_stateMachine_map_.size();
	//divide the counts in each frame by the total number of state machines in each frame
	std::vector<double>::iterator count_itr, count_end = graph_size_counts.end();
	for( count_itr = graph_size_counts.begin(); count_itr != count_end; ++count_itr )
		of << std::setprecision(5) << *count_itr/nstate_machines <<'\t';

	return true;
}