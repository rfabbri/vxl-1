//this is /contrib/bm/dsm/pro/processes/dsm_manager_write_output_process.cxx
#include<bprb/bprb_func_process.h>

#include<dsm/dsm_manager_base_sptr.h>
#include<dsm/dsm_manager.h>
#include<dsm/io/dsm_io_manager.h>
#include<dsm/dsm_pixel_time_series_map_sptr.h>

#include<vcl_iterator.h>//for distance function
#include<vcl_iostream.h>
#include<vcl_iomanip.h>
#include<vcl_sstream.h>

#include<vul/vul_file.h>

namespace dsm_manager_write_output_process_globals
{
	const unsigned int n_inputs_ = 5;
	const unsigned int n_outputs_ = 0;
}

bool dsm_manager_write_output_process_cons( bprb_func_process& pro )
{
	using namespace dsm_manager_write_output_process_globals;
	vcl_vector<vcl_string> input_types_(n_inputs_);

	unsigned i = 0;
	input_types_[i++] = "dsm_manager_base_sptr";
	input_types_[i++] = "dsm_pixel_time_series_map_sptr";
	input_types_[i++] = "unsigned";//x coordinate
	input_types_[i++] = "unsigned";//y coordinate
	input_types_[i++] = "vcl_string";//output directory

	if(!pro.set_input_types(input_types_))
        return false;

	return true;
}

bool dsm_manager_write_output_process( bprb_func_process& pro )
{
	using namespace dsm_manager_write_output_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << " dsm_manager_write_output_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned i = 0;
	dsm_manager_base_sptr manager_sptr = pro.get_input<dsm_manager_base_sptr>(i++);
	dsm_pixel_time_series_map_sptr pixel_time_series_map_sptr = pro.get_input<dsm_pixel_time_series_map_sptr>(i++);
	unsigned x = pro.get_input<unsigned>(i++);
	unsigned y = pro.get_input<unsigned>(i++);
	vcl_string output_directory = pro.get_input<vcl_string>(i++);

	dsm_state_machine_base_sptr sm_base_sptr = manager_sptr->state_machine(vgl_point_2d<unsigned>(x,y));
	dsm_time_series_sptr time_series_sptr = pixel_time_series_map_sptr->pixel_time_series_map[vgl_point_2d<unsigned>(x,y)];

	vcl_stringstream sm_directory, change_filename, state_filename;

	sm_directory << output_directory << "/sm_" << x << "_" << y;

	if( !vul_file::is_directory(sm_directory.str()) )
		vul_file::make_directory(sm_directory.str());

	//write change file
	change_filename << sm_directory.str() << "/change.txt";

	vcl_ofstream change_stream(change_filename.str().c_str());

	//vcl_map<unsigned,bool>::const_iterator c_itr, c_end = sm_base_sptr->frame_change_map().end();
	vcl_map<unsigned,bool>::const_iterator change_itr, change_end = sm_base_sptr->frame_change_map_.end();
	//for( c_itr = sm_base_sptr->frame_change_map().begin(); c_itr != c_end; ++c_itr )
	for( change_itr = sm_base_sptr->frame_change_map_.begin(); change_itr != change_end; ++change_itr )
	{
		change_stream << change_itr->first << '\t' << change_itr->second << '\n';
	}//end change map iteration

	change_stream.close();

	//write state file
	state_filename << sm_directory.str() << "/state.txt";
	
	vcl_ofstream state_stream(state_filename.str().c_str());

	vcl_map<unsigned,unsigned>::const_iterator s_itr, s_end = sm_base_sptr->frame_state_map_.end();

	for( s_itr = sm_base_sptr->frame_state_map_.begin(); s_itr != s_end; ++s_itr )
	{
		state_stream << s_itr->first << '\t' << s_itr->second << '\n';
	}//end state iteration

	state_stream.close();

	//to temporarily store the frame graph map as inferred from the frame mean map
	vcl_map<unsigned, vcl_vector<unsigned> > temp_frame_graph_map;

	//write mean files
	vcl_map<unsigned, vcl_map<unsigned, vnl_vector<double> > >::const_iterator
		mean_itr, mean_end = sm_base_sptr->frame_mean_map_.end();

	for( mean_itr = sm_base_sptr->frame_mean_map_.begin(); mean_itr != mean_end; ++mean_itr )
	{
		vcl_cout << "Writing Mean File: " << mean_itr->first << vcl_endl;

		vcl_stringstream mean_filename;

		mean_filename << sm_directory.str() << "/mean_" << vcl_setfill('0') << vcl_setw(4) << mean_itr->first << ".txt";

		vcl_ofstream mean_stream(mean_filename.str().c_str());

		vcl_map<unsigned, vnl_vector<double> >::const_iterator mean_node_itr, mean_node_end = mean_itr->second.end();

		for( mean_node_itr = mean_itr->second.begin(); mean_node_itr != mean_node_end; ++mean_node_itr )
		{
			mean_stream << mean_node_itr->first << '\t' << mean_node_itr->second[0] << '\t' << mean_node_itr->second[1] << '\n';
			temp_frame_graph_map[mean_itr->first].push_back(mean_node_itr->first);
		}//end graph iteration


		mean_stream.close();
	}//end mean iteration

	//write covariance files
	vcl_map<unsigned, vcl_map<unsigned, vnl_matrix<double> > >::const_iterator 
		frame_cov_itr, frame_cov_end = sm_base_sptr->frame_covar_map_.end();
	
	for( frame_cov_itr = sm_base_sptr->frame_covar_map_.begin(); frame_cov_itr != frame_cov_end; ++frame_cov_itr )
	{
		vcl_cout << "Writing Covariance File: " << frame_cov_itr->first << vcl_endl;

		vcl_stringstream covar_filename;

		covar_filename << sm_directory.str() << "/covar_" << vcl_setfill('0') << vcl_setw(4) << frame_cov_itr->first << ".txt";

		vcl_ofstream covar_stream(covar_filename.str().c_str());

		vcl_map<unsigned, vnl_matrix<double> >::const_iterator 
			node_covar_itr, node_covar_end = frame_cov_itr->second.end();

		for( node_covar_itr = frame_cov_itr->second.begin(); node_covar_itr != node_covar_end; ++node_covar_itr )
		{
			for( unsigned row = 0; row < 2; ++row )
			{
				for( unsigned col = 0; col < 2; ++ col )
					covar_stream << node_covar_itr->second[row][col] << '\t';
				covar_stream << '\n';
			}

		}//end graph iteration
		covar_stream.close();
	}//end covariance iteration


	//mixture weights
	vcl_map<unsigned, vcl_map<unsigned,double> >::const_iterator
		frame_weight_itr, frame_weight_end = sm_base_sptr->frame_mixture_weight_map_.end();

	for( frame_weight_itr = sm_base_sptr->frame_mixture_weight_map_.begin(); frame_weight_itr != frame_weight_end; ++frame_weight_itr )
	{
		vcl_cout << "Writing Mixture Weight File: " << frame_weight_itr->first << vcl_endl;

		vcl_stringstream weight_filename;

		weight_filename << sm_directory.str() << "/weight_" << vcl_setfill('0') << vcl_setw(4) << frame_weight_itr->first << ".txt";

		vcl_ofstream weight_stream(weight_filename.str().c_str());

		vcl_map<unsigned,double>::const_iterator
			node_weight_itr, node_weight_end = frame_weight_itr->second.end();

		for(node_weight_itr = frame_weight_itr->second.begin(); node_weight_itr != node_weight_end; ++node_weight_itr)
		{
			weight_stream << node_weight_itr->first << '\t' << node_weight_itr->second << '\n';
		}//end graph weight transition

		weight_stream.close();
	}//end weight frame iteration

	////transition probabilities

	//vcl_map<unsigned, vcl_map<unsigned, vcl_map<unsigned, unsigned> > >::const_iterator
	//	frame_transition_itr, frame_transition_end = sm_base_sptr->frame_transition_table_map_.end();

	//for(frame_transition_itr = sm_base_sptr->frame_transition_table_map().begin(); 
	//		frame_transition_itr != frame_transition_end; ++frame_transition_itr )
	//{
	//	vcl_cout << "Writing Transition Table for Frame: " << frame_transition_itr->first << vcl_endl;

	//	vcl_stringstream frame_transition_directory;

	//	frame_transition_directory << sm_directory.str() << "/transition_tables_frame_" 
	//							   << vcl_setfill('0') << vcl_setw(4) << frame_transition_itr->first;

	//	vcl_map<unsigned, vcl_map<unsigned,unsigned> >::const_iterator 
	//		node_transition_table_itr, node_transition_table_end = frame_transition_itr->second.end();

	//	for(node_transition_table_itr = frame_transition_itr->second.begin(); 
	//			node_transition_table_itr != node_transition_table_end; ++node_transition_table_itr )
	//	{
	//		vcl_stringstream transition_table_filename;

	//		transition_table_filename << frame_transition_directory.str() << "/node_"
	//			                      << vcl_setfill('0') << vcl_setw(4) << node_transition_table_itr->first;

	//		vcl_ofstream transition_table_of(transition_table_filename.str().c_str());

	//		vcl_map<unsigned,unsigned>::const_iterator 
	//			transition_table_itr, transition_table_end = node_transition_table_itr->second.end();

	//		for( transition_table_itr = node_transition_table_itr->second.begin(); 
	//				transition_table_itr != transition_table_end; ++transition_table_itr )
	//		{
	//			transition_table_of << transition_table_itr->first << '\t' << transition_table_itr->second << '\n';
	//		}

	//		transition_table_of.close();
	//	}//end node/transition table iteration
	//}//end frame transition table map
	
	//write dot files
	vcl_map<unsigned, vcl_vector<unsigned> >::const_iterator frame_graph_itr, frame_graph_end = temp_frame_graph_map.end();

	unsigned rel_time_curr;
	for( frame_graph_itr = temp_frame_graph_map.begin(); frame_graph_itr != frame_graph_end; ++frame_graph_itr )
	{
		vcl_cout << "Writing Graph File: " << frame_graph_itr->first << vcl_endl;

		vcl_stringstream dot_filename;

		dot_filename << sm_directory.str().c_str() << "/frame_" << vcl_setfill('0') << vcl_setw(4) << frame_graph_itr->first << ".dot";

		vcl_ofstream dot_of(dot_filename.str().c_str());

		if( frame_graph_itr == temp_frame_graph_map.begin() )
			rel_time_curr = 0;
		else
		{
			if( sm_base_sptr->frame_state_map_[frame_graph_itr->first] != sm_base_sptr->frame_state_map_[frame_graph_itr->first-1] )
				rel_time_curr = 0;
			else
				++rel_time_curr;
		}

		dot_of << "digraph " << sm_base_sptr->classifier_id() << "{\n"
			   << '\t' << "size = \"5,5\";\n"
			   << '\t' << "ratio=compress;\n"
			   << '\t' << "subgraph cluster_0 {\n"
			   << '\t' << "color=white;\n"; 

		dot_of << '\t' << "label = \"State Machine ID: " << sm_base_sptr->classifier_id() << "\\n"
			   << "Absolute time: " << frame_graph_itr->first << "\\n"
	           << "Relative time: " << rel_time_curr << "\\n"
	           << "Current Observation: (" << time_series_sptr->time_series[frame_graph_itr->first]->v[0]
			   << ", " << time_series_sptr->time_series[frame_graph_itr->first]->v[1] << ")\\n\" \n";
		
        //label the nodes
	    vcl_vector<unsigned>::const_iterator graph_itr, graph_end=frame_graph_itr->second.end();

		for( graph_itr = frame_graph_itr->second.begin(); graph_itr != graph_end; ++graph_itr )
		{
			vnl_vector<double> mean = sm_base_sptr->frame_mean_map()[frame_graph_itr->first][*graph_itr];

			if( *graph_itr == sm_base_sptr->frame_state_map()[frame_graph_itr->first])
			{
				
				dot_of << '\t' << '\t' << *graph_itr
					   << " [label=\"Node id: " << *graph_itr
					   << "\\n Mean: (" << mean[0] << ", " << mean[1] << ")"
					   << "\\n Relative Time: " << rel_time_curr << '\"'
					   << ", color=red];\n";
			}
			else
			{
			 dot_of << '\t' << '\t' << *graph_itr 
					<< " [label=\"Node id: " << *graph_itr 
					<< "\\n Mean: (" << mean[0] << ", "<< mean[1] << ")" << '\"'
					<< ", color=black];\n";
			}
		}//end graph iteration 1

		//label the edges
		for( graph_itr = frame_graph_itr->second.begin(); graph_itr != graph_end; ++graph_itr )
		{
			vcl_vector<unsigned>::const_iterator graph_itr2;

				for( graph_itr2 = frame_graph_itr->second.begin(); graph_itr2 != graph_end; ++graph_itr2 )
				{
					if( *graph_itr == sm_base_sptr->frame_state_map()[frame_graph_itr->first])
					{
						double transition_probability = 
							sm_base_sptr->frame_mixture_weight_map()[frame_graph_itr->first][*graph_itr2];
						dot_of << '\t' << '\t' << *graph_itr << "->"
							<< *graph_itr2 
							<< " [label=\"" << vcl_setprecision(4) << transition_probability << "\""
							<< ",color=red];\n";
					}
					else
					{
						dot_of << '\t' << '\t' 
							<<  *graph_itr << "->" << *graph_itr2
							<< " [label=\"\",color=\"0.0 0.0 0.0\"];\n";
					}
				}//end graph iteration 2 (inner)
		}//end graph iteration 2 (outter)

		dot_of << '\t' << "}\n";
		dot_of << "}";

		dot_of.close();
	}//end dot file frame/graph iteration


	//write observation files:
	vcl_map<unsigned, dsm_feature_sptr>::const_iterator 
		time_series_itr, time_series_end = time_series_sptr->time_series.end();

	for( time_series_itr = time_series_sptr->time_series.begin(); 
			time_series_itr != time_series_end; ++time_series_itr )
	{
		vcl_cout << "Writing Observation File: " << time_series_itr->first << vcl_endl;
		vcl_stringstream observation_filename;

		observation_filename << sm_directory.str() << "/observation_" << vcl_setfill('0') 
							 << vcl_setw(4) << time_series_itr->first << ".txt";

	    vcl_ofstream observation_of(observation_filename.str().c_str());

		observation_of << vcl_setprecision(5) << time_series_itr->second->v[0] << '\t' << time_series_itr->second->v[1];
	}//end time series iteration

	return true;
}//end process