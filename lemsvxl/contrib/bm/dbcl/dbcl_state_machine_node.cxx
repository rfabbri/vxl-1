//this is /contrib/bm/dbcl/dbcl_state_machine_node.cxx
#include"dbcl_state_machine_node.h"

dbcl_state_machine_node::dbcl_state_machine_node( node_id_type const& id, 
                                                  dbcl_state_machine_frame_clock_sptr frame_clock_sptr,
                                                  vnl_matrix_fixed<double,2,2> const& init_covar, 
                                                  vnl_matrix_fixed<double,2,2> const& min_covar  ):
id_(id), init_covar_(init_covar), min_covar_(min_covar), nobs_(0)
{
    node_clk_sptr_ = frame_clock_sptr;
    this->last_time_curr_ = node_clk_sptr_->time();
    transition_table_[id_][0] = 0;
    relative_time_set_.insert(0);
}//end dbcl_state_machine_node::dbcl_state_machine_node

double dbcl_state_machine_node::model_prob( vnl_vector<double> const& obs )
{
    assert( obs.size() == 2);

    return this->model_.prob_density(obs.as_ref());

}//end dbcl_state_machine_node::model_prob

double dbcl_state_machine_node::model_prob( vnl_vector_fixed<double,2> const& obs )
{
    return this->model_.prob_density(obs);
}//end dbcl_state_machine_node::model_prob

double dbcl_state_machine_node::transition_prob( node_id_type const& node_id,
                                                 time_type const& relative_time )
{
    frequency_type transition_sum = 0;

    transition_table_type::iterator table_itr = transition_table_.begin();
    transition_table_type::iterator table_end = transition_table_.end();

    unsigned num_states = transition_table_.size();
    double transition_prob = 0.0;

    //if there have been any observations at relative time
    //including all zeros.
    if( transition_table_[node_id].count(relative_time) )
    {
        for(; table_itr != table_end; ++table_itr)
            transition_sum += table_itr->second[relative_time];

        frequency_type transition_frequency = transition_table_[node_id][relative_time];

        transition_prob = (double(1 + transition_frequency) / double(num_states + transition_sum) );
    }
    else
    {
        //If there are no recorded observations at the given time,
        //assume a uniform transition distribution. Insert zeros into the unobserved time for all nodes.
        //The more general formula above will be ok if all zeros are present.
        for(; table_itr != table_end; ++table_itr )
            transition_table_[table_itr->first][relative_time] = 0;

        transition_prob = 1 / double(num_states);
    }
    return transition_prob;
}//end dbcl_state_machine_node::transition_prob

void dbcl_state_machine_node::update_model( vnl_vector<double> const& obs, time_type const& relative_time )
{
    ++this->nobs_;
    this->last_time_curr_ = node_clk_sptr_->time();
    relative_time_set_.insert(relative_time);
    bsta_update_gaussian(this->model_,1/static_cast<double>(this->nobs_),obs,this->min_covar_);
}//end dbcl_state_machine_node::update_model

void dbcl_state_machine_node::init_model( vnl_vector<double> const& obs )
{
    this->nobs_ = 1;
    this->model_.set_mean(obs.as_ref());
    this->model_.set_covar(this->init_covar_);
}

bool dbcl_state_machine_node::inc_trans_freq( node_id_type const& node_id,
                                              time_type const& relative_time )
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
            //we had no observations for the relative time therefore
            //we increased it to one observation. And insert zeros for all
            //other nodes we can oberve at this time.
            transition_table_type::iterator table_itr = transition_table_.begin();
            transition_table_type::iterator table_end = transition_table_.end();
            for(; table_itr != table_end; ++table_itr )
            {
                if( table_itr->first == node_id )
                    transition_table_[node_id][relative_time] = 1;
                else
                    transition_table_[table_itr->first][relative_time] = 0;
            }
        }
        return true;
    }
    else//there does not exist node_id in the transition table
    {
        for( time_type t = 0; t < relative_time; ++t )
            transition_table_[node_id][t] = 0;

        transition_table_[node_id][relative_time] = 1;
        return true;
    }
    return false;
}//end dbcl_state_machine_node::inc_trans_freq