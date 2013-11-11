//this is /contrib/bm/dsm/dsm_ncn.cxx
#ifndef DSM_NODE_TXX_
#define DSM_NODE_TXX_
//: file
// \file

#include"dsm_node.h"

template<int T>
dsm_node<T>::dsm_node()
{
    this->id_ = 99999;
    this->frame_clock_ptr_ = dsm_frame_clock::instance();
	this->last_time_curr_ = frame_clock_ptr_->time();
	transition_table_[id_][0] = 0;
	relative_time_set_.insert(0);
}

template<int T>
dsm_node<T>::dsm_node( unsigned const& node_id )
{
	this->id_ = node_id;
	this->frame_clock_ptr_ = dsm_frame_clock::instance();
	this->last_time_curr_ = frame_clock_ptr_->time();
	transition_table_[id_][0] = 0;
	relative_time_set_.insert(0);
}//end dsm_node<T>::dsm_node<T>

template<int T>
double dsm_node<T>::model_prob( vnl_vector<double> const& obs )
{
	assert( obs.size() == T );

	return this->model_.prob_density(obs.as_ref());
}//end dsm_node<T>::model_prob

template<int T>
double dsm_node<T>::model_prob( vnl_vector_fixed<double,T> const& obs)
{
	return this->model_.prob_density(obs);
}//end dsm_node<T>::transition_prob


template<int T>
void dsm_node<T>::init_model( vnl_vector<double> const& obs, vnl_matrix<double> const& init_covar )
{
	this->nobs_ = 1;
	this->model_.set_mean(obs.as_ref());
	vnl_matrix_fixed<double,T,T> init_covar_fixed(init_covar);
	this->model_.set_covar(init_covar_fixed);
}//end dsm_node<T>::init_model

template<int T>
void dsm_node<T>::update_model( vnl_vector<double> const& obs, unsigned const& relative_time, vnl_matrix<double> const& min_covar )
{
	++this->nobs_;
	this->last_time_curr_ = this->frame_clock_ptr_->time();
	this->relative_time_set_.insert(relative_time);
	vnl_matrix_fixed<double,T,T> min_covar_fixed(min_covar);
	bsta_update_gaussian(this->model_,1/static_cast<double>(this->nobs_),obs,min_covar_fixed);
}//end dsm_node<T>::update_model

template<int T>
void dsm_node<T>::b_write(vsl_b_ostream& os) const
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
	//vcl_map<unsigned, vcl_map<unsigned, unsigned> >::const_iterator ntf_itr, ntf_end = this->transition_table_.end();

	//for( ntf_itr = this->transition_table_.begin(); ntf_itr != ntf_end; ++ntf_itr )
	//{
	//	unsigned node_id = ntf_itr->first;
	//	vsl_b_write(os, node_id);

	//	vcl_map<unsigned, unsigned>::const_iterator tf_itr, tf_end = ntf_itr->second.end();

	//	for( tf_itr = ntf_itr->second.begin(); tf_itr != tf_end; ++tf_itr )
	//	{
	//		unsigned time=tf_itr->first;
	//		vsl_b_write(os,time);
	//		unsigned frequency=tf_itr->second;
	//		vsl_b_write(os,frequency);
	//	}

	//}//end node/time/frequency map transition

	//5. write the set of time stamps
	vsl_b_write(os, this->relative_time_set_);
	//vcl_set<unsigned>::const_iterator rts_itr, rts_end=this->relative_time_set_.end();
	//for( rts_itr = this->relative_time_set_.begin(); rts_itr != rts_end; ++rts_itr )
	//	vsl_b_write(os,*rts_itr);

	//6. write the model
	vsl_b_write(os,this->model_);

}//end dsm_node<T>::b_write

template<int T>
void dsm_node<T>::b_read(vsl_b_istream& is)
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

#define DSM_NODE_INSTANTIATE(T) \
template class dsm_node<T>

#endif //DSM_NODE_TXX_