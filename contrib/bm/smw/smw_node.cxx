//this is /contrib/bm/smw/smw_node.cxx

#include "smw/smw_node.h"

//node_id_type smw_node::next_id_ = 0;

smw_node::smw_node(node_id_type id, float init_var, float min_var):
    id_(id),
    init_var_(init_var),
    min_var_(min_var),
    nobs_(0),
    node_clk_ptr_(smw_frame_clock::instance())
    
{
    this->last_time_curr_ = node_clk_ptr_->time();
    //++next_id_;
    transition_table_[id_][0] = 0;
    relative_time_set_.insert(0);
}//end smw_node::smw_node

//returns the pvalue, the sum of the symmetrical integration of
//the observation out to the tails of the distribution.
float smw_node::model_prob(float const& obs)
{
 
    float mean = this->model_.mean();
    float dist = abs(mean - obs);
    float upper_lim = mean + dist;
    float lower_lim = mean - dist;
    float inv_prob = model_.probability(lower_lim,upper_lim);
    float prob = 1 - inv_prob;
    return prob;
 
}//end smw_node::model_prob

float smw_node::transition_prob(node_id_type const& node_id,
                                time_type const& relative_time)
{
    frequency_type transition_sum = 0;

    transition_table::iterator table_itr = transition_table_.begin();
    transition_table::iterator table_end = transition_table_.end();

    unsigned num_states = transition_table_.size();
    float transition_prob = 0.0f;

    //if there have been any observations at relative time
    //including all zeros.
    if( transition_table_[node_id].count(relative_time) )
    {
        for(; table_itr != table_end; ++table_itr)
            transition_sum += table_itr->second[relative_time];

        frequency_type transition_frequency = 
            transition_table_[node_id][relative_time];

        transition_prob = (float(1 + transition_frequency) / 
                           float(num_states + transition_sum) );
    }
    else
    {
        //If there are no recorded observations at the given time,
        //assume a uniform transition distribution. And insert zeros into
        //the unobserved time for all nodes. The more general formula
        //above will be ok if all zeros are present.
        for(; table_itr != table_end; ++table_itr)
            transition_table_[table_itr->first][relative_time] = 0;
        
        transition_prob = 1/float(num_states);
    }
    return transition_prob;
}//end smw_node::transition_probability

bool smw_node::update_model(float const& obs,time_type relative_time)
{
    ++nobs_;
    bsta_update_gaussian(model_,1/static_cast<float>(nobs_),obs,min_var_);
    last_time_curr_ = node_clk_ptr_->time();
    relative_time_set_.insert(relative_time);  
}//end smw_node::update_model

bool smw_node::init_model(float const& obs)
{
    nobs_ = 1;
    model_.set_mean(obs);
    model_.set_var(init_var_);
}//end smw_node::init_model

bool smw_node::inc_trans_freq(node_id_type node_id,
                              time_type relative_time)
{
    //if there exists observations at relative_time
    if( transition_table_.count(node_id) )
    {
        if( transition_table_[node_id].count(relative_time) )
        {
            ++transition_table_[node_id][relative_time];
        }
        else
        {
            //We had no observations for the relative time therefore
            //we increased it to one observation. And insert zeros for all
            //other nodes we can observe at this time.
            transition_table::iterator table_itr = transition_table_.begin();
            transition_table::iterator table_end = transition_table_.end();
            for(; table_itr != table_end; ++table_itr)
            {
                if( table_itr->first == node_id )
                    transition_table_[node_id][relative_time] = 1;
                else
                    transition_table_[table_itr->first][relative_time] = 0;
            }
        }
        return true;
    }
    //If there does not exist node_id in the transition table
    else
    {
        for(time_type t = 0; t < relative_time; ++t)
            transition_table_[node_id][t] = 0;
      
        transition_table_[node_id][relative_time] = 1;
       
        return true;         
    }
    return false;
}//end smw_node::increment_transition_frequency
