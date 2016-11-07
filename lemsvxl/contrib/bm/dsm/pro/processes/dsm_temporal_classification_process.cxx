//this is /contrib/bm/dsm/pro/processes/dsm_temporal_classification_process.cxx
#include<bprb/bprb_func_process.h>
#include<bprb/bprb_parameters.h>

#include<brdb/brdb_value.h>

#include<dsm/dsm_features_sptr.h>
#include<dsm/dsm_manager_base_sptr.h>
#include<dsm/dsm_state_machine.h>

#include<vcl_iostream.h>
#include<vcl_vector.h>
#include<vcl_string.h>

namespace dsm_temporal_classification_process_globals
{
	const unsigned int n_inputs_ = 1;
	const unsigned int n_outputs_ = 1;
}

bool dsm_temporal_classification_process_cons( bprb_func_process& pro )
{
	//set input/output types
	using namespace dsm_temporal_classification_process_globals;

	vcl_vector<vcl_string> input_types_(n_inputs_);
	vcl_vector<vcl_string> output_types_(n_outputs_);

	input_types_[0] = "dsm_features_sptr";
	output_types_[0] = "dsm_manager_sptr";

	if(!pro.set_input_types(input_types_))
		return false;

	if(!pro.set_output_types(output_types_))
		return false;

	return true;
}//end dsm_temporal_classification_process_cons

bool dsm_temporal_classification_process( bprb_func_process& pro )
{
	//using namespace dsm_temporal_classification_process_globals;

	//if( pro.n_inputs() < n_inputs_ )
	//{
	//	vcl_cout << pro.name() << "dsm_temporal_classification_process: The input number should be " << n_inputs_ << vcl_endl;
	//	return false;
	//}

	////get inputs
	//unsigned i = 0;
	//dsm_features_sptr features_sptr = pro.get_input<dsm_features_sptr>(i++);


	//dsm_manager_base_sptr manager_sptr = new dsm_manager();

	//vcl_map<vgl_point_2d<unsigned>, vcl_map<unsigned, vnl_vector<double> >, dsm_vgl_point_2d_coord_compare<unsigned> >::const_iterator
	//	rf_itr, rf_end = (features_sptr->reduced_feature_map()).end();
	//unsigned t_forget(30);
	//double prob_thresh(0.0002);
	//double mahalan_dist_factor(2);
	//double init_covar_d(1.0);
	//double min_covar_d(1.0);

	//pro.parameters()->get_value("t_forget",t_forget);
	//pro.parameters()->get_value("prob_thresh", prob_thresh);
	//pro.parameters()->get_value("mahalan_dist_factor", mahalan_dist_factor);
	//pro.parameters()->get_value("init_covar",init_covar_d);
	//pro.parameters()->get_value("min_covar",min_covar_d);

	//vnl_matrix<double> init_covar = vnl_diag_matrix<double>(2,init_covar_d);
	//vnl_matrix<double> min_covar = vnl_matrix<double>(2,2,min_covar_d);
	//
 //   unsigned classifier_id=0;
	//for(rf_itr = (features_sptr->reduced_feature_map()).begin(); rf_itr != rf_end; ++rf_itr )
	//{
 //       
	//	dsm_state_machine_base_sptr sm_base_sptr = new dsm_state_machine<int(2)>(classifier_id,t_forget,prob_thresh,mahalan_dist_factor,init_covar,min_covar);
 //       classifier_id++;

	//	//insert the state machine, target pair into the manager
	//	manager_sptr->insert_state_machine(rf_itr->first,sm_base_sptr);

	//	//classify the current target given all the observations
	//	vcl_map<unsigned, vnl_vector<double> >::const_iterator obs_itr, obs_end = rf_itr->second.end();
	//	for(obs_itr = rf_itr->second.begin(); obs_itr != obs_end; ++obs_itr)
	//	{
	//		sm_base_sptr->classify(obs_itr->second);
	//	}//end observation iteration
	//}//end feature sptr iteration

	//pro.set_output_val(0,manager_sptr);
    return true;
}//end dsm_temporal_classification_process