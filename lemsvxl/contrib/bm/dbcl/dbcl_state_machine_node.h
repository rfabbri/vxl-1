//this is /contrib/bm/dbcl/dbcl_state_machine_node.h
#ifndef dbcl_state_machine_node_h_
#define dbcl_state_machine_node_h_
//:
// \file
// \date August 9, 2010
// \author Brandon A. Mayer
//
// Node for state machine classifier
//
// \verbatim
//  Modifications
// \endverbatim
#include<bsta/algo/bsta_gaussian_updater.h>
#include<bsta/bsta_gaussian_full.h>

#include"dbcl_state_machine_frame_clock_sptr.h"


#include<vbl/vbl_ref_count.h>

#include<vcl_cassert.h>
#include<vcl_map.h>
#include<vcl_set.h>
#include<vcl_utility.h>

#include<vnl/vnl_diag_matrix_fixed.h>

//forward declare the classifier as a friend
class dbcl_state_machine_classifier;

class dbcl_state_machine_node:public vbl_ref_count
{
public:
    typedef bsta_gaussian_full<double,2> model_type;
    typedef unsigned node_id_type;
    typedef unsigned time_type;
    typedef unsigned frequency_type;
    typedef vcl_map<time_type,frequency_type> time_frequency_map;
    typedef vcl_map<node_id_type,time_frequency_map> transition_table_type;
    typedef vcl_set<time_type> time_set_type;

    dbcl_state_machine_node( node_id_type const& id, 
                             dbcl_state_machine_frame_clock_sptr clock_sptr,
                             vnl_matrix_fixed<double,2,2> const& init_covar = vnl_diag_matrix_fixed<double,2>(1.0).as_matrix_fixed(), 
                             vnl_matrix_fixed<double,2,2> const& min_covar = vnl_matrix_fixed<double,2,2>(0.0001) );

    ~dbcl_state_machine_node(){}

    node_id_type id(){ return id_; }

    double model_prob( vnl_vector<double> const& obs );

    double model_prob( vnl_vector_fixed<double,2> const& obs );

	transition_table_type transition_table(){ return this->transition_table_; }

    double transition_prob( node_id_type const& node_id, time_type const& relative_time );

    void update_model( vnl_vector<double> const& obs, time_type const& relative_time );

    bool inc_trans_freq( node_id_type const& node_id, time_type const& relative_time );

    vnl_vector_fixed<double,2> mean() const{ return model_.mean(); }

    vnl_matrix_fixed<double,2,2> covariance() const { return model_.covar(); }

    unsigned nobs() const { return nobs_; }

    void init_model( vnl_vector<double> const& obs );


private:
    friend dbcl_state_machine_classifier;

    model_type model_;

    node_id_type id_;

    vnl_matrix_fixed<double,2,2> init_covar_;
    
    vnl_matrix_fixed<double,2,2> min_covar_;

    unsigned nobs_;

    transition_table_type transition_table_;

    time_set_type relative_time_set_;

    time_type last_time_curr_;

    dbcl_state_machine_frame_clock_sptr node_clk_sptr_;
};

#endif //dbcl_state_machine_node_h_