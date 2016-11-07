//this is /contrib/bm/dsm/pro/processes/dsm_state_machine_classify_pixel_time_series_map_process.cxx
#include<bprb/bprb_func_process.h>

#include<dsm/dsm_manager_base_sptr.h>
#include<dsm/dsm_manager.h>
#include<dsm/dsm_pixel_time_series_map_sptr.h>

#include<vcl_iterator.h>//for distance function
#include<vcl_iostream.h>
#include<vcl_iomanip.h>
#include<vcl_sstream.h>

#include<vul/vul_file.h>


namespace dsm_state_machine_classify_pixel_time_series_map_process_globals
{
	const unsigned int n_inputs_ = 8;
	const unsigned int n_outputs_ = 1;
}

bool dsm_state_machine_classify_pixel_time_series_map_process_cons( bprb_func_process& pro )
{
	using namespace dsm_state_machine_classify_pixel_time_series_map_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

	unsigned i = 0;
	input_types_[i++] = "vcl_string";//results directory
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

bool dsm_state_machine_classify_pixel_time_series_map_process( bprb_func_process& pro )
{
	using namespace dsm_state_machine_classify_pixel_time_series_map_process_globals;

	if( pro.n_inputs() < n_inputs_ )
	{
		vcl_cout << pro.name() << " dsm_state_machine_classify_pixel_time_series_map_process: The input number should be " << n_inputs_ << vcl_endl;
		return false;
	}

	//get inputs
	unsigned  i = 0;
	vcl_string result_dir = pro.get_input<vcl_string>(i++);
	dsm_pixel_time_series_map_sptr pixel_time_series_map_sptr = pro.get_input<dsm_pixel_time_series_map_sptr>(i++);
	unsigned ndims = pro.get_input<unsigned>(i++);
	unsigned t_forget = pro.get_input<unsigned>(i++);
	double prob_thresh = pro.get_input<double>(i++);
	double mahalan_factor = pro.get_input<double>(i++);
	double dinit_covar = pro.get_input<double>(i++);
	double dmin_covar = pro.get_input<double>(i++);

	dsm_manager_base_sptr manager_base_sptr;

	if( pixel_time_series_map_sptr->pixel_time_series_map.empty() )
	{
		vcl_cerr << "---- ERROR: dsm_state_machine_classify_pixel_time_series_map, the pixel time series map is empty. ----" << vcl_endl;
		return false;
	}

	switch(ndims)
	{
	case 2:
		{
			vcl_cout << "Using a 2d State Machine." << vcl_endl;

			vnl_matrix<double> init_covar = vnl_diag_matrix<double>(ndims, dinit_covar);
			//vnl_matrix<double> min_covar(ndims,ndims,dmin_covar);
			vnl_matrix<double> min_covar = vnl_diag_matrix<double>(ndims, dmin_covar);

			manager_base_sptr = new dsm_manager<2>();

			dsm_pixel_time_series_map::pixel_time_series_map_type::const_iterator 
				t_itr, t_end = pixel_time_series_map_sptr->pixel_time_series_map.end();
			unsigned state_machine_id;
			for( state_machine_id = 0, t_itr = pixel_time_series_map_sptr->pixel_time_series_map.begin(); t_itr != t_end; ++t_itr, ++state_machine_id )
			{

				vcl_cout << "Creating State Machine " << state_machine_id << " for target: (" << t_itr->first.x() << "," << t_itr->first.y() << ")" << vcl_endl;

				dsm_state_machine_base_sptr state_machine_base_sptr = new dsm_state_machine<2>( state_machine_id,
																							    t_forget, prob_thresh,
																								mahalan_factor,
																								init_covar, min_covar);

				manager_base_sptr->insert_state_machine(t_itr->first,state_machine_base_sptr);

				vcl_map<unsigned, dsm_feature_sptr>::const_iterator f_itr, f_end = t_itr->second->time_series.end();
				unsigned time_series_size = t_itr->second->time_series.size();


				vcl_stringstream output_directory, change_filename, state_filename;

				output_directory << result_dir << "/sm_" << state_machine_base_sptr->classifier_id();

				if( !vul_file::is_directory(output_directory.str()) )
						vul_file::make_directory(output_directory.str());

				change_filename << output_directory.str() << "/change.txt";

				vcl_ofstream change_of( change_filename.str().c_str(), vcl_ios::out ) ;

				state_filename << output_directory.str() << "/state.txt";

				vcl_ofstream state_of( state_filename.str().c_str(), vcl_ios::out );

				for( f_itr = t_itr->second->time_series.begin(); f_itr != f_end; ++f_itr )
				{
					
					bool change = state_machine_base_sptr->classify(f_itr->second->v);

					change_of << f_itr->second->t << '\t' << change << '\n';

					state_of << f_itr->second->t << '\t' << state_machine_base_sptr->curr_node_id() << '\n';

					vcl_cout << t_itr->second->time_series.size() - distance(f_itr,f_end) << " out of " << time_series_size << " points updated." << vcl_endl;

					vcl_cout << "Writing Graph Dot File: " << vcl_endl;
					
					vcl_stringstream dot_filename;

					dot_filename << output_directory.str() << "/frame_"<< vcl_setfill('0') << vcl_setw(4) << f_itr->second->t << ".dot";

					vcl_ofstream of( dot_filename.str().c_str(), vcl_ios::out );

					of << "digraph " << state_machine_base_sptr->classifier_id() << "{\n"
					   << '\t' << "size = \"5,5\";\n"
					   << '\t' << "ratio=compress;\n"
					   << '\t' << "subgraph cluster_0 {\n"
					   << '\t' << "color=lightgrey;\n";

					of << '\t' << "label = \"State Machine ID: " << state_machine_base_sptr->classifier_id() << "\\n"
					   << "Absolute time: " << f_itr->second->t << "\\n"
					   << "Relative time: " << state_machine_base_sptr->rel_time_curr() << "\\n"
					   << "Current Observation: (";

					for( unsigned i = 0; i < f_itr->second->v.size(); ++i)
					{
						if( i < f_itr->second->v.size() - 1 )
							of << f_itr->second->v[i] << ", ";
						else
							of << f_itr->second->v[i] << ")\\n \n";
					}//end feature dimension iteration
					
					vcl_map<unsigned, dsm_node_base_sptr>::const_iterator g_itr, g_end = state_machine_base_sptr->graph_end();

					//label the nodes
					for( g_itr = state_machine_base_sptr->graph_begin(); g_itr != g_end; ++g_itr )
					{
						dsm_node<2>* node_ptr = static_cast<dsm_node<2>*>(g_itr->second.as_pointer());
						vnl_vector<double> mean = node_ptr->mean();

						if( g_itr == state_machine_base_sptr->curr_node_itr() )
						 of << '\t' << '\t' << g_itr->first 
							<< " [label=\"Node id: " << g_itr->first
							<< "\\n Mean: (" << mean[0] << ", " << mean[1] << ")"
							<< "\\n Relative Time: " << state_machine_base_sptr->rel_time_curr() + 1 << '\"'
							<< ", color=red]\n";
						else
						 of << '\t' << '\t' << g_itr->first 
							<< " [label=\"Node id: " << g_itr->first 
							<< "\\n Mean: (" << mean[0] << ", "<< mean[1] << ")" << '\"'
							<< ", color=black];\n";

					}//end graph iteration

					

					//label the edges
					for( g_itr = state_machine_base_sptr->graph_begin(); g_itr != g_end; ++g_itr )
					{
						vcl_map<unsigned, dsm_node_base_sptr>::const_iterator g_itr2, g_end2 = state_machine_base_sptr->graph_end();

						for(g_itr2 = state_machine_base_sptr->graph_begin(); g_itr2 != g_end2; ++g_itr2)
						{
							if( g_itr == state_machine_base_sptr->curr_node_itr() )
							{
								double t_prob = g_itr->second->transition_prob(g_itr2->first,state_machine_base_sptr->rel_time_curr());
								of << '\t' << '\t' << g_itr->first << "->"
								   << g_itr2->first 
								   << " [label=\"" << vcl_setprecision(4) << t_prob << "\""
								   << ",color=red];\n";
							}
							else
							{
								of << '\t' << '\t' 
								   <<  g_itr->first << "->" << g_itr2->first
								   << " [label=\"\",color=\"0.0 0.0 0.0\"];\n";
							}
						}//end inner graph iteration

					}//end outter graph iteration

					of << '\t' << "}\n";
					of << "}";

					of.close();

					vcl_stringstream mean_filename, covar_filename, observation_filename, weight_filename;

					mean_filename << output_directory.str() << "/mean_" << vcl_setfill('0') << vcl_setw(4) << f_itr->second->t << ".txt";
					covar_filename << output_directory.str() << "/covar_" << vcl_setfill('0') << vcl_setw(4) << f_itr->second->t << ".txt";
					observation_filename << output_directory.str() << "/observation_" << vcl_setfill('0') << vcl_setw(4) << f_itr->second->t << ".txt";
					weight_filename << output_directory.str() << "/weight_"  << vcl_setfill('0') << vcl_setw(4) << f_itr->second->t << ".txt";
					vcl_string mean_filename_str = mean_filename.str();
					vcl_string covar_filename_str = covar_filename.str();
					vcl_string observation_filename_str = observation_filename.str();
					vcl_ofstream mof(mean_filename.str().c_str());
					vcl_ofstream cof(covar_filename.str().c_str());
					vcl_ofstream oof(observation_filename.str().c_str());
					vcl_ofstream wof(weight_filename.str().c_str());
					oof << vcl_setprecision(5) << f_itr->second->v[0] << '\t' << f_itr->second->v[1] << '\n';
					oof.close();
					//write the means of all nodes
					vcl_cout << "Writing Mean, Covariance and Weight Files... " << vcl_endl;
					for( g_itr = state_machine_base_sptr->graph_begin(); g_itr != g_end; ++g_itr )
					{

						unsigned node_id = g_itr->first;
						dsm_node<2>* node_ptr = static_cast<dsm_node<2>*>(g_itr->second.as_pointer());
						
						//write mean files
						vnl_vector<double> mean = node_ptr->mean();
						mof << node_id << '\t' << vcl_setprecision(5) << mean[0] << '\t' << mean [1] << '\n';
												
						//write covariance files
						vnl_matrix<double> covar = node_ptr->covariance();
						for(unsigned row = 0; row < 2; ++row)
						{
							for(unsigned col = 0; col < 2; ++col)
								cof << covar[row][col] << '\t';
							cof << '\n';
						}
						

						//write transition probability files
						double t_prob = state_machine_base_sptr->curr_node_itr()->second->transition_prob(g_itr->first,state_machine_base_sptr->rel_time_curr());
						wof << node_id << '\t' << vcl_setprecision(8) << t_prob << '\n';

					}//end graph iteration

					mof.close();
					cof.close();
					wof.close();	

				}//end time series iteration
			
				change_of.close();
				state_of.close();
			}//end target iteration

		}//end case 2
		break;
	default:
		{
			vcl_cerr << "---- ERROR PROCESS DSM_STATE_MACHINE_CLASSIFY_PIXEL_TIME_SERIES_MAP_PROCESS:\n"
				     << "     no implmentation for ndims = " << ndims << '\n' << vcl_flush;
			return false;
		}//end default
	}//end switch

	pro.set_output_val(0,manager_base_sptr);
	return true;
}