//this is /contrib/bm/dsm/dsm_node.h
#ifndef DSM_NODE_H_
#define DSM_NODE_H_

#include<bsta/algo/bsta_gaussian_updater.h>
#include<bsta/bsta_gaussian_full.h>
#include<bsta/io/bsta_io_gaussian_full.h>
#include<bsta/bsta_gaussian_sphere.h>
#include<bsta/io/bsta_io_gaussian_sphere.h>

#include"dsm_frame_clock.h"
#include"dsm_node_base_sptr.h"
#include"dsm_map_utilities.h"

#include<vcl_cassert.h>
#include<vcl_map.h>
#include<vcl_set.h>
#include<vcl_utility.h>

#include<vnl/vnl_matrix_fixed.h>
#include<vnl/io/vnl_io_matrix_fixed.h>

#include<vsl/vsl_binary_io.h>
#include<vsl/vsl_map_io.h>
#include<vsl/vsl_set_io.h>

//forward declare the state machine as a friend so it may have direct
//access to private members
template<int T>
class dsm_state_machine;

template<int T=2>
class dsm_node: public dsm_node_base
{
public:
	dsm_node();

	dsm_node( unsigned const& node_id/*, 
                   vnl_matrix_fixed<double,T,T> const& init_covar, 
                   vnl_matrix_fixed<double,T,T> const& min_covar*/ );

	~dsm_node(){}

	unsigned id(){ return this->id_; }

        virtual int model_dim(){return T;}

	virtual double model_prob( vnl_vector<double> const& obs );

        virtual void init_model( vnl_vector<double> const& obs, 
                                 vnl_matrix<double> const& init_covar );

	virtual void update_model( vnl_vector<double> const& obs, 
                                   unsigned const& relative_time, 
                                   vnl_matrix<double> const& min_covar );

	virtual void b_write(vsl_b_ostream& os) const; 

	virtual void b_read(vsl_b_istream& is);

	double sqr_mahalanobis_dist( vnl_vector<double> const& v )
        { return this->model_.sqr_mahalanobis_dist(v); }

	double model_prob( vnl_vector_fixed<double,T> const& obs );

	vnl_vector_fixed<double,T> mean() const { return this->model_.mean(); }

	vnl_matrix_fixed<double,T,T> covariance() const 
        { return this->model_.covar(); }

protected:
	template<int> friend class dsm_state_machine;

	bsta_gaussian_full<double,T> model_;

	//vnl_matrix_fixed<double, T, T> init_covar_;

	//vnl_matrix_fixed<double, T, T> min_covar_;
};


template<>
class dsm_node<1>: public dsm_node_base
{
public:
	dsm_node();

	dsm_node( unsigned const& node_id/*, 
                  vnl_matrix_fixed<double,T,T> const& init_covar, 
                  vnl_matrix_fixed<double,T,T> const& min_covar*/ );

	~dsm_node(){}

	unsigned id(){ return this->id_; }

        virtual int model_dim(){return 1;}

	virtual double model_prob( vnl_vector<double> const& obs );

    virtual void init_model( vnl_vector<double> const& obs, vnl_matrix<double> const& init_covar );

	virtual void update_model( vnl_vector<double> const& obs, unsigned const& relative_time, vnl_matrix<double> const& min_covar );

	virtual void b_write(vsl_b_ostream& os) const; 

	virtual void b_read(vsl_b_istream& is);

	double sqr_mahalanobis_dist( vnl_vector<double> const& v ){ return this->model_.sqr_mahalanobis_dist(v[0]); }

	double model_prob( vnl_vector_fixed<double,1> const& obs );

	vnl_vector_fixed<double,1> mean() const { return vnl_vector_fixed<double,1>(this->model_.mean()); }

	vnl_matrix_fixed<double,1,1> covariance() const { return vnl_matrix_fixed<double,1,1>(this->model_.covar()); }

protected:
	friend class dsm_state_machine<1>;

	bsta_gaussian_sphere<double,1> model_;

	//vnl_matrix_fixed<double, T, T> init_covar_;

	//vnl_matrix_fixed<double, T, T> min_covar_;
};
#endif //DSM_NODE_H_
