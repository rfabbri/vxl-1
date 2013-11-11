////this is /contrib/bm/dsm2/algo/dsm2_state_machine_updater_mahalan.txx
//#ifndef DSM2_STATE_MACHINE_UPDATER_MAHALAN_TXX_
//#define DSM2_STATE_MACHINE_UPDATER_MAHALAN_TXX_
//#include<dsm2/algo/dsm2_state_machine_updater_mahalan.h>
//
//template<class stateMachineT>
//bool dsm2_state_machine_updater_mahalan<stateMachineT>::update( 
//	dsm2_state_machine_base_sptr& state_machine_base_sptr,
//		dsm2_feature_base_sptr const& feature_base_sptr)
//{
//	if( state_machine_base_sptr->graph.empty() )
//	{
//		state_machine_base_sptr->next_node_id = 0;
//
//		dsm2_num_obs<node_class_type>* num_obs_ptr = new dsm2_num_obs<node_class_type>();
//
//		dsm2_feature<math_type,dimension>* feature_ptr = 
//			static_cast<dsm2_feature<math_type,dimension>*>(feature_base_sptr.as_pointer());
//
//		num_obs_ptr->set_covar(this->init_std);
//
//		num_obs_ptr->set_mean(feature_ptr->v);
//
//		dsm2_node_base_sptr node_base_sptr(num_obs_ptr);
//
//		//insert the new node into the graph
//		vcl_pair<vcl_map<unsigned, dsm2_node_base_sptr>::iterator, bool> ret;
//		ret = state_machine_base_sptr->graph.insert(vcl_pair<unsigned,dsm2_node_base_sptr>
//										(state_machine_base_sptr->next_node_id++,node_base_sptr));
//
//		state_machine_base_sptr->curr_node_itr = ret.first;
//	}//end check if graph is empty
//	else //graph is not empty
//	{
//		dsm2_feature<math_type,dimension>* feature_ptr = 
//			static_cast<dsm2_feature<math_type,dimension>*>(feature_base_sptr.as_pointer());
//
//		vcl_map<double, vcl_map<unsigned, dsm2_node_base_sptr>::iterator > weight_node_map;
//		vcl_map<unsigned, dsm2_node_base_sptr>::iterator most_prob_itr, 
//			g_itr, g_end = state_machine_base_sptr->graph.end();
//
//		for( g_itr = state_machine_base_sptr->graph.begin();
//				g_itr != g_end; ++g_itr )
//		{
//			node_class_type* node_ptr = static_cast<node_class_type*>(g_itr->second.as_pointer());
//
//			double curr_sqr_mahalan_dist = node_ptr->sqr_mahalanobis_dist(feature_ptr->v);
//
//			//if( curr_sqr_mahalan_dist < this->mahalan_thresh )
//			//{
//
//			//}//end if dist < mahalan_thresh
//		}//end graph iteration
//
//	}//graph is not empty
//	return true;
//}//end update
//
//template<class stateMachineT>
//typename dsm2_state_machine_updater_mahalan<stateMachineT>::graph_type_iterator
//	dsm2_state_machine_updater_mahalan<stateMachineT>::add_node( dsm2_state_machine_base_sptr& state_machine_base_sptr,
//					dsm2_feature_base_sptr const& feature_base_sptr )
//{
//	node_base_sptr node_sptr = new node_class_type;
//
//	vcl_pair<graph_type_iterator,bool> ret = 
//		state_machine_base_sptr->graph.insert(
//			vcl_pair<unsigned,node_base_sptr>(state_machine_base_sptr->next_node_id++, node_sptr) );
//
//	transition_class_type* transition_ptr = static_cast<transition_class_type*>(state_machine_base_sptr.as_pointer());
//
//		
//
//
//	return ret.first;
//}
//
//#define DSM2_STATE_MACHINE_UPDATER_MAHALAN_INSTANTIATE(...)\
//template class dsm2_state_machine_updater_mahalan<__VA_ARGS__>
//
//#endif //DSM2_STATE_MACHINE_UPDATER_MAHALAN_TXX_