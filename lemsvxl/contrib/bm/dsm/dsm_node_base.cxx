//this is /contrib/bm/dsm/dsm_no_base.cxx

#include"dsm_node_base.h"

dsm_node_base::dsm_node_base( unsigned const& node_id ):id_(node_id)
{
	this->frame_clock_ptr_ = dsm_frame_clock::instance();
	this->last_time_curr_ = frame_clock_ptr_->time();
	transition_table_[id_][0] = 0;
	relative_time_set_.insert(0);
}//end dsm_node_base::dsm_node_base

double dsm_node_base::transition_prob(unsigned const& node_id, unsigned const& relative_time )
{
	unsigned transition_sum = 0;

	vcl_map<unsigned, vcl_map<unsigned, unsigned> >::iterator table_itr, table_end = this->transition_table_.end();

	unsigned num_states = this->transition_table_.size();
	double transition_prob = 0.0;

	//if there have been any observations at relative time
	//including all zeros.
	if( this->transition_table_[node_id].count(relative_time) )
	{
		for( table_itr = this->transition_table_.begin(); table_itr != table_end; ++table_itr )
			transition_sum += table_itr->second[relative_time];

		unsigned transition_frequency = transition_table_[node_id][relative_time];

		transition_prob = (double(1+transition_frequency)/double(num_states + transition_sum));
			
	}
	else
	{
		//If there are no recorded observations at the given time,
		//assume a uniform transition distribution. Insert zeros into the unobserved time for all nodes.
		//The more general formula above will be ok if all zeros are present.
		for( table_itr = this->transition_table_.begin(); table_itr != table_end; ++table_itr )
			this->transition_table_[table_itr->first][relative_time] = 0;

		transition_prob = 1/double(num_states);
	}

	return transition_prob;
}//end dsm_node_base::transition_prob

bool dsm_node_base::inc_trans_freq( unsigned const& node_id, unsigned const& relative_time )
{
	//if there exists observations at relative_time
	if( this->transition_table_.count(node_id) )
	{
		if( this->transition_table_[node_id].count(relative_time) )
		{
			++this->transition_table_[node_id][relative_time];
		}
		else
		{
			//we had no observations for the relative time therefore
			//we increased it to one observation. And insert zeros for all
			//other nodes we can observe at this time.
			vcl_map<unsigned, vcl_map<unsigned, unsigned> >::iterator table_itr, table_end = this->transition_table_.end();
		
			for( table_itr = this->transition_table_.begin(); table_itr != table_end; ++table_itr )
			{
				if( table_itr->first == node_id )
					this->transition_table_[node_id][relative_time] = 1;
				else
					transition_table_[table_itr->first][relative_time] = 0;
			}
		}
		return true;
	}
	else //there does not exist node_id in the transition table
	{
		for( unsigned t= 0; t < relative_time; ++t )
			this->transition_table_[node_id][t] = 0;

		this->transition_table_[node_id][relative_time] = 1;

		return true;
	}

	return false;
}//end dsm_node_base::inc_trans_freq