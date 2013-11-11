//this is /contrib/bm/dsm/Templates/dsm_node+1-.cxx
#include"../dsm_node.h"

dsm_node<1>::dsm_node()
{
    this->id_ = 99999;
    this->frame_clock_ptr_ = dsm_frame_clock::instance();
	this->last_time_curr_ = frame_clock_ptr_->time();
	transition_table_[id_][0] = 0;
	relative_time_set_.insert(0);
}


dsm_node<1>::dsm_node( unsigned const& node_id )
{
	this->id_ = node_id;
	this->frame_clock_ptr_ = dsm_frame_clock::instance();
	this->last_time_curr_ = frame_clock_ptr_->time();
	transition_table_[id_][0] = 0;
	relative_time_set_.insert(0);
}//end dsm_node<T>::dsm_node<T>


double dsm_node<1>::model_prob( vnl_vector<double> const& obs )
{
	assert( obs.size() == 1 );

	return this->model_.prob_density(obs[0]);
}//end dsm_node<T>::model_prob


double dsm_node<1>::model_prob( vnl_vector_fixed<double,1> const& obs)
{
	return this->model_.prob_density(obs[0]);
}//end dsm_node<T>::transition_prob

void dsm_node<1>::init_model( vnl_vector<double> const& obs, vnl_matrix<double> const& init_covar )
{
	this->nobs_ = 1;
	this->model_.set_mean(obs[0]);
	vnl_matrix_fixed<double,1,1> init_covar_fixed(init_covar);
	this->model_.set_covar(init_covar_fixed[0][0]);
}//end dsm_node<T>::init_model

void dsm_node<1>::update_model( vnl_vector<double> const& obs, unsigned const& relative_time, vnl_matrix<double> const& min_covar )
{
	++this->nobs_;
	this->last_time_curr_ = this->frame_clock_ptr_->time();
	this->relative_time_set_.insert(relative_time);
	vnl_matrix_fixed<double,1,1> min_covar_fixed(min_covar);
	bsta_update_gaussian(this->model_,1/static_cast<double>(this->nobs_),obs[0],min_covar_fixed[0][0]);
}//end dsm_node<T>::update_model

void dsm_node<1>::b_write(vsl_b_ostream& os) const
{
	const short version_no = 1;
	vsl_b_write(os, version_no);

	//1. write this node's id
	vsl_b_write(os, this->id_);

	//2. write this node's last_time_curr
	vsl_b_write(os, this->last_time_curr_);

	//3. write the number of observations
	vsl_b_write(os, this->nobs_);

	//4. write this node's node/time/frequency map
	vsl_b_write(os, this->transition_table_);

	//5. write the set of time stamps
	vsl_b_write(os, this->relative_time_set_);
	//vcl_set<unsigned>::const_iterator rts_itr, rts_end=this->relative_time_set_.end();
	//for( rts_itr = this->relative_time_set_.begin(); rts_itr != rts_end; ++rts_itr )
	//	vsl_b_write(os,*rts_itr);

	//6. write the model
	vsl_b_write(os,this->model_);

}//end dsm_node<T>::b_write

void dsm_node<1>::b_read(vsl_b_istream& is)
{
	if(!is) return;

	short v;
	vsl_b_read(is,v);

	switch(v)
	{
	case 1:
		{
			vsl_b_read(is,this->id_);
			vsl_b_read(is,this->last_time_curr_);
			vsl_b_read(is,this->nobs_);
			vsl_b_read(is,this->transition_table_);
			vsl_b_read(is,this->relative_time_set_);
			vsl_b_read(is,this->model_);
			break;
		}//end case1
	default:
		{
			vcl_cerr << "ERROR: dsm_node::b_read() -- unknown version number." << vcl_flush;
			return;
		}
	}

	return;
}//end dsm_node<T>::b_read

