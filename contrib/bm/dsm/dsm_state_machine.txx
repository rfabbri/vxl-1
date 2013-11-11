//this is /contrib/bm/dsm/dsm_state_machine.txx
#ifndef DSM_STATE_MACHINE_TXX_
#define DSM_STATE_MACHINE_TXX_

#include"dsm_state_machine.h"

template<int T>
vcl_map<unsigned, dsm_node_base_sptr>::iterator dsm_state_machine<T>::add_node()
{
	//create the new node
	dsm_node_base_sptr new_node = new dsm_node<T>(this->next_node_id_);
    ++this->next_node_id_;

    vcl_pair<vcl_map<unsigned,dsm_node_base_sptr>::iterator,bool> ret;
    //insert node into graph.
	ret = this->graph_.insert( vcl_make_pair(new_node->id(),new_node) );

    //Traverse the graph and adjust the transition tables of every
    //node to include the new node id with appropriate modifications.
    vcl_map<unsigned,dsm_node_base_sptr>::iterator g_itr, g_end =this->graph_.end();

    vcl_set<unsigned>::iterator rel_time_set_itr, rel_time_set_end;

    for( g_itr = this->graph_.begin(); g_itr != g_end; ++g_itr )
    {
        ret.first->second->transition_table_[g_itr->first][0] = 0;
      
        rel_time_set_end = g_itr->second->relative_time_set_.end();
        for(rel_time_set_itr = g_itr->second->relative_time_set_.begin(); rel_time_set_itr != rel_time_set_end; ++rel_time_set_itr )
            g_itr->second->transition_table_[new_node->id()][*rel_time_set_itr] = 0;
    }//end graph iteration

    return ret.first;
}//end dsm_state_machien::add_node()

template<int T>
bool dsm_state_machine<T>::remove_node( unsigned const& node_id )
{
    //Temporarily save the position elements so we don't hve to run
    //find twice, to check for existence then to erase.

    vcl_map<unsigned,dsm_node_base_sptr>::iterator g_element = this->graph_.find(node_id);

    if( g_element != this->graph_.end() )
    {
        this->graph_.erase(g_element);

        vcl_map<unsigned,dsm_node_base_sptr>::iterator g_itr, g_end = this->graph_.end();

        for( g_itr = this->graph_.begin(); g_itr != g_end; ++g_itr )
            g_itr->second->transition_table_.erase(node_id);

        return true;
    }
    else
    {
       vcl_cerr << "ERROR: dsm_state_machine::remove_node \n"
                << "node_id: " << node_id << '\n'
                << "Not found in the classifier's graph. " << vcl_flush;
        return false;
    }
    
}//end dsm_state_machine::remove_node

template<int T>
void dsm_state_machine<T>::prune_graph()
{
    vcl_map<unsigned,dsm_node_base_sptr>::iterator g_itr, g_end = this->graph_.end();

    vcl_set<unsigned> nodes_to_delete;

    unsigned absolute_time = this->frame_clock_ptr_->time();
    unsigned diff;

    for(g_itr = this->graph_.begin(); g_itr != g_end; ++g_itr)
    {
        unsigned last_time_curr = g_itr->second->last_time_curr_;
        unsigned graph_size = this->size();
        diff = absolute_time - g_itr->second->last_time_curr_;

        if( diff > t_forget_ )
            nodes_to_delete.insert(g_itr->first);

    }//end graph iteration

    vcl_set<unsigned>::iterator nodes_to_delete_itr, nodes_to_delete_end = nodes_to_delete.end();

    for( nodes_to_delete_itr = nodes_to_delete.begin(); nodes_to_delete_itr != nodes_to_delete_end; ++nodes_to_delete_itr )
        this->remove_node(*nodes_to_delete_itr);

}//end dsm_state_machine<T>::prune_graph()

template<int T>
bool dsm_state_machine<T>::update_manhalanobis_distance( vnl_vector<double> const& obs )
{
	if( this->graph_.empty() )
	{
		vcl_map<unsigned,dsm_node_base_sptr>::iterator new_node_itr = this->add_node();
		new_node_itr->second->init_model(obs,this->init_covar_);		
		new_node_itr->second->last_time_curr_ = this->frame_clock_ptr_->time();
	}
	else //the graph is not empty
	{
		vcl_map<unsigned,dsm_node_base_sptr>::iterator g_itr, g_end = this->graph_.end();
		vcl_map<double,dsm_node_base_sptr> weight_node_map;

		for( g_itr = this->graph_.begin(); g_itr != g_end; ++g_itr )
		{	
			double transition_prob = this->curr_node_itr_->second->transition_prob(g_itr->first, this->rel_time_curr_);
			double model_prob = g_itr->second->model_prob(obs);	

			dsm_node<T>* node_ptr = static_cast<dsm_node<T>*>(g_itr->second.ptr());
			//if( node_ptr->model_.sqr_mahalanobis_dist(obs) < mahalan_dist_factor_ )
			if( node_ptr->sqr_mahalanobis_dist(obs) < mahalan_dist_factor_ );
			{
				double covar_det = node_ptr->model_.det_covar();
				weight_node_map.insert(vcl_make_pair(transition_prob/covar_det,g_itr->second));
			}
		}//end graph iteration

		if( !weight_node_map.empty() ) //there is a posteriori above the threshold
		{
			//keys are stored in a vcl_map in order from lowest to highest.
			dsm_node_base_sptr node_base_sptr = weight_node_map.begin()->second;
			node_base_sptr->update_model(obs,0,this->min_covar_);
		}
		else //we need a new node.
		{
			vcl_map<unsigned,dsm_node_base_sptr>::iterator new_node_itr = this->add_node();
			new_node_itr->second->init_model(obs,this->init_covar_);
			new_node_itr->second->last_time_curr_ = this->frame_clock_ptr_->time();
		}
	}//end graph is empty check
	vcl_map<unsigned, dsm_node_base_sptr>::iterator g_itr, g_end = this->graph_.end();
	for( g_itr = this->graph_.begin(); g_itr != g_end; ++g_itr )
	{
		dsm_node<T>* node_ptr = static_cast<dsm_node<T>*>(g_itr->second.as_pointer());
		this->frame_mean_map_[this->frame_clock_ptr_->time()][g_itr->first] = node_ptr->mean();
		this->frame_covar_map_[this->frame_clock_ptr_->time()][g_itr->first] = node_ptr->covariance();
	}//end graph iteration
	return true;
}//end dsm_state_machine<T>::update_manalanobis_distance

template<int T>
bool dsm_state_machine<T>::classify( vnl_vector<double> const& obs )
{
	if( this->graph_.empty() )
	{
		this->curr_node_itr_ = this->add_node();
		this->curr_node_id_ = this->curr_node_itr_->first;
		this->rel_time_curr_ = 0;
		this->curr_node_itr_->second->init_model(obs, this->init_covar_);
		this->curr_node_itr_->second->last_time_curr_ = this->frame_clock_ptr_->time();
		this->target_visited_.insert(this->curr_node_id_);
	}
	else //graph is not empty
	{
		//check if any nodes need to be deleted due to to_forget_
		this->prune_graph();
		
		vcl_map<double, vcl_map<unsigned, dsm_node_base_sptr>::iterator> weight_node_map;
		vcl_map<unsigned, dsm_node_base_sptr>::iterator most_prob_itr, g_itr, g_end = this->graph_.end();
		
		for( g_itr = this->graph_.begin(); g_itr != g_end; ++g_itr )
		{
			dsm_node<T>* node_ptr = static_cast<dsm_node<T>*>(g_itr->second.as_pointer());

			//double curr_sqr_mahalanobis_distance = node_ptr->model_.sqr_mahalanobis_dist(obs);
			double curr_sqr_mahalanobis_distance = node_ptr->sqr_mahalanobis_dist(obs);
			if( curr_sqr_mahalanobis_distance < this->mahalan_dist_factor_ )
			{	
				//double transition_prob = node_ptr->transition_prob(g_itr->first, this->rel_time_curr_);
				double transition_prob = this->curr_node_itr_->second->transition_prob(g_itr->first, this->rel_time_curr_);
				double covar_det = node_ptr->model_.det_covar();
				weight_node_map.insert( vcl_make_pair( transition_prob/covar_det, g_itr ) );
			}
		}//end graph iteration	

		if( !weight_node_map.empty() )
		{
			vcl_map<unsigned, dsm_node_base_sptr>::iterator most_prob_itr = weight_node_map.begin()->second;
			unsigned most_prob_id = most_prob_itr->first;

			this->curr_node_itr_->second->inc_trans_freq(most_prob_id, rel_time_curr_);

			//if the most probable node is the current
			if( most_prob_itr == this->curr_node_itr_)
				++this->rel_time_curr_;
			else //we have moved to a new node
			{
				this->rel_time_curr_ = 0;
				this->curr_node_itr_ = most_prob_itr;
				this->curr_node_id_ = most_prob_id;
			}

			this->curr_node_itr_->second->update_model(obs,this->rel_time_curr_,this->min_covar_);
		}
		else //the point was not within mahalan_factor of any distribution, need a new node
		{
			vcl_map<unsigned, dsm_node_base_sptr>::iterator old_curr_itr = this->curr_node_itr_;
			this->curr_node_itr_ = this->add_node();
			this->curr_node_id_ = this->curr_node_itr_->first;

			//fix the old node's transition table
			old_curr_itr->second->inc_trans_freq(this->curr_node_id_, rel_time_curr_);

			this->rel_time_curr_ = 0;

			//initialize the new model
			this->curr_node_itr_->second->init_model(obs, this->init_covar_);
		}		
	}

	vcl_set<unsigned>::iterator itr = this->target_visited_.find(this->curr_node_id_);

	if( itr == this->target_visited_.end() ) //the node hasn't been visited by the target
		this->change_ = true;
	else
		this->change_ = false;

	this->target_visited_.insert(this->curr_node_id_);
	this->frame_change_map_.insert( vcl_make_pair( this->frame_clock_ptr_->time(), this->change_ ) );
	this->frame_state_map_.insert( vcl_make_pair( this->frame_clock_ptr_->time(), this->curr_node_id_ ) );
	
	//update the mean and covariance maps
	vcl_map<unsigned, dsm_node_base_sptr>::iterator g_itr, g_end = this->graph_.end();
	for( g_itr = this->graph_.begin(); g_itr != g_end; ++g_itr )
	{
		dsm_node<T>* node_ptr = static_cast<dsm_node<T>*>(g_itr->second.as_pointer());
		this->frame_mean_map_[this->frame_clock_ptr_->time()][g_itr->first] = node_ptr->mean();
		this->frame_covar_map_[this->frame_clock_ptr_->time()][g_itr->first] = node_ptr->covariance();
		double transition_prob = this->curr_node_itr_->second->transition_prob(g_itr->first,this->rel_time_curr_);
		this->frame_covar_map_[this->frame_clock_ptr_->time()][g_itr->first] = transition_prob;
	}//end graph iteration

	return this->change_;

}//end dsm_state_machine<T>::classify


template<int T>
bool dsm_state_machine<T>::classify(dsm_feature_sptr feature_sptr)
{
	if( this->graph_.empty() )
	{
		this->curr_node_itr_ = this->add_node();
		this->curr_node_id_ = this->curr_node_itr_->first;
		this->rel_time_curr_ = 0;
		this->curr_node_itr_->second->init_model(feature_sptr->v, this->init_covar_);
		this->curr_node_itr_->second->last_time_curr_ = feature_sptr->t;
		this->target_visited_.insert(this->curr_node_id_);
		this->frame_mixture_weight_map_[feature_sptr->t][this->curr_node_id_] = 1.0;
	}
	else //graph is not empty
	{
		this->prune_graph();

		vcl_map<double, vcl_map<unsigned, dsm_node_base_sptr>::iterator > weight_node_map;
		vcl_map<unsigned, dsm_node_base_sptr>::iterator most_prob_itr, g_itr, g_end = this->graph_.end();

		for( g_itr = this->graph_.begin(); g_itr != g_end; ++g_itr )
		{
			dsm_node<T>* node_ptr = static_cast<dsm_node<T>*>(g_itr->second.as_pointer());

			//double curr_sqr_mahalanobis_distance = node_ptr->model_.sqr_mahalanobis_dist(feature_sptr->v);
			double curr_sqr_mahalanobis_distance = node_ptr->sqr_mahalanobis_dist(feature_sptr->v);
			
			if( curr_sqr_mahalanobis_distance < this->mahalan_dist_factor_ )
			{
				//double transition_prob = node_ptr->transition_prob(g_itr->first, this->rel_time_curr_);
				//the transition probability from the current node to the query node
				double transition_prob = this->curr_node_itr_->second->transition_prob(g_itr->first, this->rel_time_curr_);
				double covar_det = node_ptr->model_.det_covar();
				//not that vcl_log is the natrual logarithm not base 10
				double discriminant = -.5*curr_sqr_mahalanobis_distance + vcl_log(transition_prob) - .5*covar_det;
				//weight_node_map.insert( vcl_make_pair( transition_prob/covar_det, g_itr ) );
				weight_node_map.insert(vcl_make_pair(discriminant, g_itr) );
			}
		}//end graph iteration
		
		if( !weight_node_map.empty() )//ther are nodes that are within the threshold
		{
			//vcl_map<unsigned, dsm_node_base_sptr>::iterator most_prob_itr = weight_node_map.begin()->second;
			vcl_map<unsigned, dsm_node_base_sptr>::iterator most_prob_itr = weight_node_map.rbegin()->second;
			unsigned most_prob_id = most_prob_itr->first;

			this->curr_node_itr_->second->inc_trans_freq(most_prob_id, rel_time_curr_);

			//if the most probable node is the current
			if( most_prob_itr == this->curr_node_itr_ )
				++this->rel_time_curr_;
			else//we have moved to a new node
			{
				this->rel_time_curr_ = 0;
				this->curr_node_itr_ = most_prob_itr;
				this->curr_node_id_ = most_prob_id;
			}

			this->curr_node_itr_->second->update_model(feature_sptr->v,this->rel_time_curr_,this->min_covar_);
		}//end if(!weight_node_map.empty())
		else//the point wass not within mahalan_factor of any distribution
		{
			vcl_map<unsigned, dsm_node_base_sptr>::iterator old_curr_itr = this->curr_node_itr_;
			this->curr_node_itr_ = this->add_node();
			this->curr_node_id_ = this->curr_node_itr_->first;

			//fix the old node's transition table
			old_curr_itr->second->inc_trans_freq(this->curr_node_id_, rel_time_curr_);

			this->rel_time_curr_ = 0;
			
			//initialize the new node's model
			this->curr_node_itr_->second->init_model(feature_sptr->v,this->init_covar_);
		} //end else the point not within mahalan_factor
	}//end else graph is not empty


	vcl_set<unsigned>::iterator itr = this->target_visited_.find(this->curr_node_id_);
	
	if( itr == this->target_visited_.end() ) //the node hasn't been visited by the target
		this->change_ = true;
	else
		this->change_ = false;

	this->target_visited_.insert(this->curr_node_id_);
	this->frame_change_map_.insert( vcl_make_pair( feature_sptr->t, this->change_ ) );
	this->frame_state_map_.insert( vcl_make_pair( feature_sptr->t, this->curr_node_id_ ) );
	//update the mean and covariance maps
	vcl_map<unsigned, dsm_node_base_sptr>::iterator g_itr, g_end = this->graph_.end();
	for( g_itr = this->graph_.begin(); g_itr != g_end; ++g_itr )
	{
		dsm_node<T>* node_ptr = static_cast<dsm_node<T>*>(g_itr->second.as_pointer());
		this->frame_mean_map_[feature_sptr->t][g_itr->first] = node_ptr->mean();
		this->frame_covar_map_[feature_sptr->t][g_itr->first] = node_ptr->covariance();
		double transition_prob = this->curr_node_itr_->second->transition_prob(g_itr->first,this->rel_time_curr_);
		this->frame_mixture_weight_map_[feature_sptr->t][g_itr->first] = transition_prob;
	}//end graph iteration

	return this->change_;
}//end dsm_state_machine<T>::classify(dsm_feature_sptr ) 

template<int T>
void dsm_state_machine<T>::b_write(vsl_b_ostream& os) const
{
	unsigned short version_no = 1;
	vsl_b_write(os,version_no);

	// ----- WRITE THE DATA MEMBERS FROM THE BASE  -----
	//1. write classifier id
	vsl_b_write(os, this->classifier_id_);

	//2. write next node id
	vsl_b_write(os, this->next_node_id_);

	//3. write the graph
	vsl_b_write(os, this->graph_.size());
    for( vcl_map<unsigned, dsm_node_base_sptr>::const_iterator itr = this->graph_.begin(); itr != this->graph_.end(); ++itr )
    {
        vsl_b_write(os,(*itr).first);
        (*itr).second->b_write(os);
    }//end graph iteration

	//4. write curr_node_id
	vsl_b_write(os, this->curr_node_id_);

	//5. write rel_time_curr
	vsl_b_write(os, this->rel_time_curr_);

	//6. write t_forget
	vsl_b_write(os, this->t_forget_);

	//7. write mahalan_dist_factor
	vsl_b_write(os, this->mahalan_dist_factor_);

	//8. write the curr_obs
	vsl_b_write(os,curr_obs_);

	//9. write the change bit
	vsl_b_write(os, this->change_);

	//10. write the max_prob
	vsl_b_write(os, this->max_prob_);

	//11. write prob_thresh
	vsl_b_write(os, this->prob_thresh_);

    vsl_b_write (os, this->frame_transition_table_map_);

	//12. write the frame_change_map
	vsl_b_write(os, this->frame_change_map_);

	//13. write the frame_state_map
	vsl_b_write(os, this->frame_state_map_);

	//14. write the frame mixture weight map
	vsl_b_write(os, this->frame_mixture_weight_map_);

	//15. write the set of target visted states
	vsl_b_write(os, this->target_visited_);


	// ----- WRITE THE DATA MEMBERS FROM THE DERIVED CLASS  -----

	//16. write the minimum covariance
	vsl_b_write(os, this->min_covar_);

	//17. write the initial covariance
	vsl_b_write(os, this->init_covar_);

	//18. write the frame mean map
	unsigned nframes_mean_map = frame_mean_map_.size();
	vsl_b_write(os, nframes_mean_map);
	vcl_map<unsigned, vcl_map<unsigned, vnl_vector<double> > >::const_iterator 
		fmm_itr, fmm_end = this->frame_mean_map_.end();

	for( fmm_itr = this->frame_mean_map_.begin(); fmm_itr != fmm_end; ++fmm_itr )
	{
		//write the frame number
		vsl_b_write(os,fmm_itr->first);


		//write the number of nodes
		vsl_b_write(os,fmm_itr->second.size());
		
		vcl_map<unsigned, vnl_vector<double> >::const_iterator n_itr, n_end = fmm_itr->second.end();

		for( n_itr = fmm_itr->second.begin(); n_itr != n_end; ++n_itr )
		{
			//write the node id
			vsl_b_write(os, n_itr->first);
			
			//write the mean vector
			vsl_b_write(os, n_itr->second);
		}//end node iteration
	}//end frame iteration
	
	//19. write the frame_covar_map
	unsigned nframe_covar_map = this->frame_covar_map_.size();
	vsl_b_write(os, nframe_covar_map);
	vcl_map<unsigned, vcl_map<unsigned, vnl_matrix<double> > >::const_iterator 
		fcm_itr, fcm_end = this->frame_covar_map_.end();

	for( fcm_itr = this->frame_covar_map_.begin(); fcm_itr != fcm_end; ++fcm_itr )
	{
		//write the frame number
		vsl_b_write(os,fcm_itr->first);

		//write the number of nodes
		vsl_b_write(os,fcm_itr->second.size());

		vcl_map<unsigned, vnl_matrix<double> >::const_iterator
			n_itr, n_end = fcm_itr->second.end();

		for( n_itr = fcm_itr->second.begin(); n_itr != n_end; ++n_itr )
		{
			//write node id
			vsl_b_write(os, n_itr->first);

			//write the covariance matrix
			vsl_b_write(os, n_itr->second);
		}//end node iteration
	}//end frame iteration

}//end dsm_state_machine<T>::b_write

template<int T>
void dsm_state_machine<T>::b_read(vsl_b_istream& is)
{
    if(!is) return;

    short ver;
    vsl_b_read(is,ver);

    switch(ver)
    {
    case 1:
        {
            vsl_b_read(is, this->classifier_id_);
            vsl_b_read(is, this->next_node_id_);
            
            unsigned graph_size;
            vsl_b_read(is, graph_size);
            for(unsigned i = 0; i < graph_size; ++i)
            {
                unsigned key;
                vsl_b_read(is,key);
                dsm_node_base_sptr value = new dsm_node<T>();
                value->b_read(is);
                this->graph_[key] = value;
            }//end graph iteration

            vsl_b_read(is, this->curr_node_id_);
            vsl_b_read(is, this->rel_time_curr_);
            vsl_b_read(is, this->t_forget_);
            vsl_b_read(is, this->mahalan_dist_factor_);
            vsl_b_read(is, this->curr_obs_);
            vsl_b_read(is, this->change_);
            vsl_b_read(is, this->max_prob_);
            vsl_b_read(is, this->prob_thresh_);
            vsl_b_read(is, this->frame_transition_table_map_);
            vsl_b_read(is, this->frame_change_map_);
            vsl_b_read(is, this->frame_state_map_);
            vsl_b_read(is, this->frame_mixture_weight_map_);
            vsl_b_read(is, this->target_visited_);

            vsl_b_read(is,this->min_covar_);
            vsl_b_read(is,this->init_covar_);

			//read the frame mean map
			unsigned nframes_mean_map;
			vsl_b_read(is, nframes_mean_map);

			for(unsigned frame = 0; frame < nframes_mean_map; ++frame)
			{
				//read the frame number
				unsigned frame_number;
				vsl_b_read(is, frame_number);

				//read the number of nodes
				unsigned num_nodes;
				vsl_b_read(is, num_nodes);

				vcl_map<unsigned, vnl_vector<double> > node_mean_map;
				
				for( unsigned node_idx = 0; node_idx < num_nodes; ++node_idx )
				{
					//read the node id
					unsigned node_id;
					vsl_b_read(is, node_id);

					//read the mean vector
					vnl_vector<double> mean;
					vsl_b_read(is, mean);
					node_mean_map[node_id] = mean;
				}//end node iteration

				//insert into the frame/node/mean map
				this->frame_mean_map_[frame_number] = node_mean_map;
			}//end frame iteration

			//read the frame covariance map
			unsigned nframes_covar_map;
			vsl_b_read(is, nframes_covar_map);

			for( unsigned frame = 0; frame < nframes_covar_map; ++frame )
			{
				//read the frame number
				unsigned frame_number;
				vsl_b_read(is, frame_number);

				//read the number of nodes
				unsigned num_nodes;
				vsl_b_read(is, num_nodes);

				vcl_map<unsigned, vnl_matrix<double> > node_covar_map;
				for( unsigned node_idx = 0; node_idx < num_nodes; ++node_idx )
				{
					//read node id
					unsigned node_id;
					vsl_b_read(is, node_id);

					//read the covariance matrix
					vnl_matrix<double> covar_mat;
					vsl_b_read(is, covar_mat);
					
					node_covar_map[node_id] = covar_mat;

				}//end node iteration

				this->frame_covar_map_[frame_number] = node_covar_map;
			}//end frame iteration

        }//end case 1
        break;
    default:
        {
            vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, dsm_state_machine<T>&)\n"
                << "           Unknown version number "<< ver << '\n';
            is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
            return;
        }//end default
    }//end switch
}//end dsm_state_machine<T>::b_read

template<int T>
void dsm_state_machine<T>::write_dot_file( vcl_string const& filename)
{

}//end dsm_state_machine<T>::write_dot_file

template<int T>
void dsm_state_machine<T>::write_dot_file_full( vcl_string const& filename )
{
}



#define DSM_STATE_MACHINE_INSTANTIATE(T) \
template class dsm_state_machine<T>

#endif //DSM_STATE_MACHINE_TXX_

