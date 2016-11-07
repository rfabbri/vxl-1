//this is /contrib/bm/smw/smw_node_gaussian_frames.cxx

#include"smw/smw_node_gaussian_frames.h"

smw_node_gaussian_frames::smw_node_gaussian_frames():id_(next_id_),nobs_(0),
node_clk_ptr_(smw_frame_clock::instance())
{
    this->last_curr_time_ = node_clk_ptr_->time();
    ++next_id_;
    //Add an edge connecting the node to itself in the transition table
    transition_table_[id_][0] = 0;
}

float smw_node_gaussian_frames::model_probability(float const& obs)
{
    return this->model_.prob_density(obs);
}//end smw_node_gaussian_frames::model_prob

float smw_node_gaussian_frames::transition_probability(node_id_type node_id,
                                                       time_type relative_time)
{
    frequency_type transition_sum = 0;

    transition_table::iterator table_itr = transition_table_.begin();
    transition_table::iterator table_end = transition_table_.end();

    unsigned num_states = transition_table_.size();
    
    if( transition_table_[node_id].count(relative_time) )
    {
        for(; table_itr != table_end; ++table_itr)
            transition_sum += table_itr->second[relative_time];
    
        frequency_type transition_frequency = 
            transition_table_[node_id][relative_time];

        return ( (1 + transition_frequency) /  (num_states + transition_sum) );
    }
    else
    {
        //If there are no recorded observations at the given time, 
        //assume a uniform transition distribution. And insert zeros into
        //the unobserved time. The more general formula above will be ok
        //if all zeros are present.
        for(; table_itr != table_end; ++table_itr)
            transition_table_[table_itr->first][relative_time] = 0;

        observed_time_set_.insert(relative_time);
        
        return 1/num_states;
    }
}//end smw_node_gaussian_frames::transition_probability

bool smw_node_gaussian_frames::initialize_model(float const& obs,
                                                float const& init_var)
{
    nobs_ = 1;
    model_.set_mean(obs);
    model_.set_var(init_var);
}

void smw_node_gaussian_frames::update_model(float const& obs)
{
    bsta_update_gaussian(model_,1/nobs_,obs,0.0004f);
}
