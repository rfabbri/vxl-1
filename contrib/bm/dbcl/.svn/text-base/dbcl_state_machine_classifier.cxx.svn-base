//this is /contrib/bm/dbcl/dbcl_state_machine_classifier.cxx

#include"dbcl_state_machine_classifier.h"

dbcl_state_machine_classifier::dbcl_state_machine_classifier( dbcl_classifier::feature_map_type training_data,
															  dbcl_classifier::feature_map_type test_data,
                                                              time_type const& t_forget,
                                                              double const& prob_thresh,
															  double const& mahalan_dist_factor,
                                                              vnl_matrix_fixed<double,2,2> const& init_covar,  
                                                              vnl_matrix_fixed<double,2,2> const& min_covar ):
t_forget_(t_forget), prob_thresh_(prob_thresh), mahalan_dist_factor_(mahalan_dist_factor), init_covar_(init_covar),min_covar_(min_covar), change_(false), node_id_counter_(0),
classifier_clk_sptr_(new dbcl_state_machine_frame_clock)
{
	this->training_data_ = training_data;
    this->test_data_ = test_data;
}//end dbcl_state_machine_classifier::dbcl_state_machine_classifier

dbcl_state_machine_classifier::graph_type::iterator dbcl_state_machine_classifier::add_node()
{
    //create the new node
    dbcl_state_machine_node_sptr new_node = new dbcl_state_machine_node(this->node_id_counter_,this->classifier_clk_sptr_,this->init_covar_,this->min_covar_);
    ++this->node_id_counter_;
    node_id_type new_node_id = new_node->id();
    graph_type::iterator new_node_itr;

    vcl_pair<graph_type::iterator,bool> ret;
    //insert node into the graph
    ret = this->graph_.insert( vcl_make_pair(new_node_id,new_node) );

    //Must traverse the graph and adjust the transition tables of every 
    //node to include the new node id with appropriate modifications.
    graph_type::iterator g_itr;
    graph_type::iterator g_end = this->graph_.end();
    time_set_type::iterator t_itr;
    time_set_type::iterator t_end;

    for( g_itr = this->graph_.begin(); g_itr != g_end; ++g_itr )
    {
        ret.first->second->transition_table_[g_itr->first][0] = 0;

        t_itr = g_itr->second->relative_time_set_.begin();
        t_end = g_itr->second->relative_time_set_.end();
        for(; t_itr != t_end; ++t_itr)
            g_itr->second->transition_table_[new_node_id][*t_itr] = 0;
    }//end graph iteration

    return ret.first;
}//end dbcl_state_machine_classifier::add_node()

bool dbcl_state_machine_classifier::remove_node( node_id_type const& node_id )
{
    //Temporarily save the position elements so we don't hve to run
    //find twice, to check for existence then to erase.

    unsigned node_id_debud = node_id;

    graph_type::iterator g_element = this->graph_.find(node_id);

    if( g_element != this->graph_.end() )
    {
        this->graph_.erase(g_element);
        //now must traverse and adjust transition tables
        graph_type::iterator g_itr;
        graph_type::iterator g_end = this->graph_.end();

        for( g_itr = this->graph_.begin(); g_itr != g_end; ++g_itr )
            g_itr->second->transition_table_.erase(node_id);
    }
    else
    {
       vcl_cerr << "ERROR: dbcl_state_machine_classifier::remove_node \n"
                << "node_id: " << node_id << '\n'
                << "Not found in the classifier's graph. " << vcl_flush;
        return false;
    }

    return true;
}//end dbcl_state_machine_classifier::remove_node

void dbcl_state_machine_classifier::prune_graph()
{

		graph_type::iterator g_itr = this->graph_.begin();
		graph_type::iterator g_end = this->graph_.end();

		unsigned g_first = g_itr->first;

		time_type absolute_time = classifier_clk_sptr_->time();
		time_type diff;

		for(; g_itr != g_end; ++g_itr )
		{
			unsigned last_time_curr = g_itr->second->last_time_curr_;
			unsigned graph_size = this->size();
			diff = absolute_time - g_itr->second->last_time_curr_;

			if( diff > t_forget_ )
			{
				this->remove_node(g_itr->first);
				//iterators will become obsolete afer call to
				//remove as it will call erase on the graph structure.
				g_itr = this->graph_.begin();
				g_end = this->graph_.end();
			}
		}//end graph iteration

}//end dbcl_state_machine_classifier::prune_graph()

//THIS VERSION IS NO LONGER USED
bool dbcl_state_machine_classifier::update( vnl_vector<double> const& obs )
{
    curr_obs_ = obs;

    if( this->graph_.empty() ) //If the graph is empty
    {
        this->curr_node_itr_ = this->add_node();
        this->curr_node_id_ = curr_node_itr_->first;
        this->rel_time_curr_ = 0;
        this->curr_node_itr_->second->init_model(obs);
        this->curr_node_itr_->second->last_time_curr_ = this->classifier_clk_sptr_->time();
    }
    else//graph is not empty
    {
		//Check if any nodes need to be deleted due to t_forget_
		//and delete if appropriate.
		this->prune_graph();
        this->change_ = false;
        graph_type::iterator g_itr = this->graph_.begin();
        graph_type::iterator g_end = this->graph_.end();
        graph_type::iterator most_prob_itr;
        node_id_type most_prob_id;
        double max_prob = 0.0, temp_prob = 0.0f;
        double model_prob = 0.0, transition_prob = 0.0;

        for(; g_itr != g_end; ++g_itr)
        {
            transition_prob = this->curr_node_itr_->second->transition_prob(g_itr->first,this->rel_time_curr_);

            model_prob = g_itr->second->model_prob(obs);

            //we take the square root to compute the geometric
            //mean of the product of random variables
            temp_prob = vcl_sqrt(transition_prob*model_prob);

            if( temp_prob > max_prob )
            {
                max_prob = temp_prob;
                most_prob_itr = g_itr;
                most_prob_id = g_itr->first;
            }

        }//end graph iteration
        max_prob_ = max_prob;
		this->frame_max_prob_map_[this->classifier_clk_sptr_->curr_time_] = max_prob_;

        if( max_prob > prob_thresh_ )
        {
            this->change_ = false;
            this->curr_node_itr_->second->inc_trans_freq(most_prob_id,rel_time_curr_);

            //if the most probable node is the current
            if( most_prob_itr == this->curr_node_itr_ )
                ++this->rel_time_curr_;
            else//we have moved to a new node
            {
                this->rel_time_curr_ = 0;
                this->curr_node_itr_ = most_prob_itr;
                this->curr_node_id_ = most_prob_id;
            }
            
            curr_node_itr_->second->update_model(obs,rel_time_curr_);
        }//end max_prob > prob_thresh
        else //max_prob < prob_thresh_
        {
            //The posterior falls below our threshold, therefore the
            //current state machine model doesn't explain the observation and there
            //is a change.
            this->change_ = 1;
            //Need to create a new node.
            graph_type::iterator old_curr_itr = this->curr_node_itr_;
            this->curr_node_itr_ = this->add_node();
            this->curr_node_id_ = this->curr_node_itr_->first;

            //Fix the old node's transition table
            old_curr_itr->second->inc_trans_freq(this->curr_node_id_,rel_time_curr_);

            this->rel_time_curr_ = 0;

            //initialize the new model
            this->curr_node_itr_->second->init_model(obs);
        }// end max_prob < prob_thresh_
    }//end check if graph is empty

    this->curr_node_itr_->second->last_time_curr_ = this->classifier_clk_sptr_->time();
    return true;
}//end dbcl_state_machine_classifier::update

bool dbcl_state_machine_classifier::update_manhalanobis_distance( vnl_vector<double> const& obs )
{
    curr_obs_ = obs;

    if( this->graph_.empty() ) //If the graph is empty
    {
        this->curr_node_itr_ = this->add_node();
        this->curr_node_id_ = curr_node_itr_->first;
        this->rel_time_curr_ = 0;
        this->curr_node_itr_->second->init_model(obs);
        this->curr_node_itr_->second->last_time_curr_ = this->classifier_clk_sptr_->time();
    }
    else//graph is not empty
    {
		//Check if any nodes need to be deleted due to t_forget_
		//and delete if appropriate.
		this->prune_graph();
        this->change_ = false;
        graph_type::iterator g_itr = this->graph_.begin();
        graph_type::iterator g_end = this->graph_.end();
        graph_type::iterator most_prob_itr;
        node_id_type most_prob_id;
        double max_prob = 0.0, temp_prob = 0.0f;
        double model_prob = 0.0, transition_prob = 0.0;

		vcl_map<double,graph_type::iterator> weight_node_map;
        for(; g_itr != g_end; ++g_itr)
        {
            transition_prob = this->curr_node_itr_->second->transition_prob(g_itr->first,this->rel_time_curr_);

            model_prob = g_itr->second->model_prob(obs);

			double curr_sqr_mahalanobis_distance = g_itr->second->model_.sqr_mahalanobis_dist(obs);

			//double thresh =  this->mahalan_dist_factor_*vcl_sqrt(g_itr->second->model_.det_covar());

			if( curr_sqr_mahalanobis_distance < mahalan_dist_factor_ )
			{
				double covar_det = g_itr->second->model_.det_covar();
				weight_node_map.insert(vcl_make_pair(transition_prob/covar_det,g_itr));
			}

        }//end graph iteration
        //max_prob_ = max_prob;
		//this->frame_max_prob_map_[this->classifier_clk_sptr_->curr_time_] = max_prob_;

        if( !weight_node_map.empty() )
        {
			most_prob_itr = (weight_node_map.begin())->second;
			most_prob_id = most_prob_itr->first;
			
            //this->change_ = false;
            this->curr_node_itr_->second->inc_trans_freq(most_prob_id,rel_time_curr_);

            //if the most probable node is the current
            if( most_prob_itr == this->curr_node_itr_ )
                ++this->rel_time_curr_;
            else//we have moved to a new node
            {
                this->rel_time_curr_ = 0;
                this->curr_node_itr_ = most_prob_itr;
                this->curr_node_id_ = most_prob_id;
            }
            
            curr_node_itr_->second->update_model(obs,rel_time_curr_);
        }//end max_prob > prob_thresh
        else //the point was not within 2 stdv of any distributions
        {
            //The posterior falls below our threshold, therefore the
            //current state machine model doesn't explain the observation and there
            //is a change.
            //this->change_ = 1;
            //Need to create a new node.
            graph_type::iterator old_curr_itr = this->curr_node_itr_;
            this->curr_node_itr_ = this->add_node();
            this->curr_node_id_ = this->curr_node_itr_->first;	

            //Fix the old node's transition table
            old_curr_itr->second->inc_trans_freq(this->curr_node_id_,rel_time_curr_);

            this->rel_time_curr_ = 0;

            //initialize the new model
            this->curr_node_itr_->second->init_model(obs);
        }// end max_prob < prob_thresh_
    }//end check if graph is empty

    this->curr_node_itr_->second->last_time_curr_ = this->classifier_clk_sptr_->time();
    return true;
}//end dbcl_state_machine_classifier::update_manhalanobis

void dbcl_state_machine_classifier::classify()
{
    //reset the clock to zero
    this->classifier_clk_sptr_->reset_clock();

    //the assumption is that the data stream is contiguous (starts from 0 and is not missing frames).
    unsigned num_frames = this->test_data_.size();

	for( unsigned t = 0; t < num_frames; ++t )
	{

		if( this->training_data_.find(t) != this->training_data_.end() )
		{
			dbcl_temporal_feature_sptr curr_training_sptr = this->training_data_[t];
			dbcl_temporal_feature_sptr curr_test_sptr = this->test_data_[t];
			//vnl_vector<double> feature_vector = curr_feature_sptr->feature_vector();
			//unsigned feature_vector_size = feature_vector.size();

			//the update function
			this->update_manhalanobis_distance(curr_training_sptr->feature_vector());

			this->classify(curr_test_sptr);

			unsigned label = static_cast<unsigned>(this->change_);
			//curr_feature_sptr->set_label(label) ;
			this->frame_change_map_[t] = this->change_;
			this->frame_training_state_map_[t] = curr_node_id_;
			this->frame_test_state_map_[t] = curr_test_sptr->label();

			//this->frame_transition_table_map_[t] = this->curr_node_itr_->second->transition_table();


			//THIS CODE JUST RECORDS THE MEAN AND COVARANCES OF ALL NODES IN THE GRAPH AT THIS TIME
			graph_type::iterator graph_itr;
			graph_type::iterator graph_end = this->graph_.end();

			vcl_vector<vnl_vector_fixed<double,2> > curr_means;
			vcl_vector<vnl_matrix_fixed<double,2,2> > curr_covars;

			//vcl_map<unsigned,vnl_vector<double> >
			node_id_mean_map_type node_id_mean_map;

			//vcl_map<unsigned,vnl_matrix<double> >
			node_id_covar_map_type node_id_covar_map;
			node_id_mixture_weight_map_type node_id_mixture_weight_map;
			for( graph_itr = this->graph_.begin(); graph_itr != graph_end; ++graph_itr )
			{
				node_id_mean_map[graph_itr->first] = graph_itr->second->mean();
				node_id_covar_map[graph_itr->first] = graph_itr->second->covariance();
				node_id_mixture_weight_map[graph_itr->first] = this->curr_node_itr_->second->transition_prob(graph_itr->first,this->rel_time_curr_);
			}//end graph iteration

			this->frame_mean_map_[t] = node_id_mean_map;
			this->frame_covar_map_[t] = node_id_covar_map;
			this->frame_mixture_weight_map_[t] = node_id_mixture_weight_map;

			//INCREMENT THE CLOCK
			classifier_clk_sptr_->increment_time();
		}
		else
			vcl_cerr << "dbcl_state_machine_classifier::classify() - couldn't find feature vector for frame " << t << vcl_endl;
	}//end frame iteration

}//end dbcl_state_machine_classifier

bool dbcl_state_machine_classifier::classify( dbcl_temporal_feature_sptr feature_sptr )
{
	graph_type::iterator g_itr;
	graph_type::iterator g_end = this->graph_.end();

	vnl_vector<double> obs = feature_sptr->feature_vector();

	vcl_map<double,graph_type::iterator> weight_node_map;

	vcl_map<double,graph_type::iterator> discriminant_node_map;

	this->change_ =  true;

	for( g_itr = this->graph_.begin(); g_itr != g_end; ++g_itr )
	{
		double curr_mahalanobis_distance = g_itr->second->model_.sqr_mahalanobis_dist(obs);
		double curr_transition_prob = this->curr_node_itr_->second->transition_prob(g_itr->first,this->rel_time_curr_);
		double curr_covar_det = g_itr->second->model_.det_covar();

		double curr_discriminant = -.5*curr_mahalanobis_distance + vcl_log(curr_transition_prob) - .5*vcl_log(curr_covar_det);

		//log(p(x|wi)p(wi))
		discriminant_node_map.insert( vcl_make_pair(curr_discriminant,g_itr) );

		if( curr_mahalanobis_distance < 5/*mahalan_dist_factor_*/ )
		{
			this->change_ = false;
		}

	}//end graph iteration

	unsigned most_likely_node_id = discriminant_node_map.rbegin()->second->second->id();

	feature_sptr->set_label(most_likely_node_id);

	return this->change_;


}//end dbcl_state_machine_classifier::classify( vnl_vector<double> const& obs )