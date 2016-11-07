//this is /contrib/bm/dbcl/dbcl_state_machine_classifier.h
#ifndef DBCL_STATE_MACHINE_CLASSIFIER_H_
#define DBCL_STATE_MACHINE_CLASSIFIER_H_
//:
// \file
// \date August 9, 2010
// \author Brandon A. Mayer
//
// This classifier uses a state machine to classify changes. This class is a replacement for
// the smw_graph class in the smw library
//
// \verbatim
//  Modifications
// \endverbatim

#include"dbcl_classifier.h"

#include"dbcl_state_machine_frame_clock_sptr.h"
#include"dbcl_state_machine_node_sptr.h"


#include"dncn_factory.h"


class dbcl_state_machine_classifier:public dbcl_classifier
{
public:
    typedef vcl_map< unsigned, dbcl_state_machine_node_sptr > graph_type;

    typedef dbcl_state_machine_node::time_type time_type;

    typedef dbcl_state_machine_node::node_id_type node_id_type;

    typedef dbcl_state_machine_node::time_set_type time_set_type;

	typedef dbcl_state_machine_node::transition_table_type transition_table_type;

	typedef vcl_map<node_id_type,vnl_vector<double> > node_id_mean_map_type;

	typedef vcl_map< time_type, node_id_mean_map_type > frame_mean_map_type;

	typedef vcl_map< node_id_type,vnl_matrix<double> > node_id_covar_map_type;

	typedef vcl_map< time_type, node_id_covar_map_type > frame_covar_map_type;

	typedef vcl_map< time_type, transition_table_type > frame_transition_table_map_type;

	typedef vcl_map< node_id_type, double > node_id_mixture_weight_map_type;

	typedef vcl_map< time_type, node_id_mixture_weight_map_type > frame_mixture_weight_map_type;

	typedef vcl_map< time_type, double > frame_max_prob_map_type;

	typedef vcl_map<unsigned, unsigned> frame_state_map_type;


	
    //There is no concept of training/testing data for this classifier therefore we will store all data in test data to 
    //be used in building the model while classifying each point
    dbcl_state_machine_classifier( dbcl_classifier::feature_map_type training_data,
								   dbcl_classifier::feature_map_type test_data,
                                   time_type const& t_forget = 30,
                                   double const& prob_thresh = 0.0002,
								   double const& mahalan_dist_factor = 1,
                                   vnl_matrix_fixed<double,2,2> const& init_covar = vnl_diag_matrix_fixed<double,2>(1.0).as_matrix_fixed(), 
                                   vnl_matrix_fixed<double,2,2> const& min_covar = vnl_matrix_fixed<double,2,2>(0.0001) );

    ~dbcl_state_machine_classifier(){}

    virtual void classify();

    bool update( vnl_vector<double> const& obs );

	bool update_manhalanobis_distance( vnl_vector<double> const& obs );

	bool classify( dbcl_temporal_feature_sptr feature_sptr );

    bool change(){ return change_; }

    unsigned size(){ return graph_.size(); }

    //print a summary of the graph
    //friend vcl_ostream& operator<<(vcl_ostream& os, dbcl_state_machine_classifier&);

    double max_prob() const { return max_prob_; }

    dbcl_state_machine_frame_clock_sptr clock_sptr(){ return this->classifier_clk_sptr_; }

    vcl_map<unsigned, bool> frame_change_map(){ return this->frame_change_map_; }

	frame_mean_map_type frame_mean_map(){ return this->frame_mean_map_; }

	frame_covar_map_type frame_covar_map(){ return this->frame_covar_map_; }

	frame_transition_table_map_type frame_transition_table_map(){ return this->frame_transition_table_map_; }

	frame_mixture_weight_map_type frame_mixture_weight_map(){ return this->frame_mixture_weight_map_; }

	frame_max_prob_map_type frame_max_prob_map(){ return this->frame_max_prob_map_; }

	unsigned nframes(){ return this->frame_change_map_.size(); }

	frame_state_map_type frame_test_state_map(){ return this->frame_test_state_map_; }

	frame_state_map_type frame_training_state_map(){ return this->frame_training_state_map_; }

	//setters
	void set_t_forget( time_type const& t_forget){ this->t_forget_ = t_forget; }

	void set_prob_thresh( double const& prob_thresh){ this->prob_thresh_ = prob_thresh; }

	void set_init_covar( vnl_matrix_fixed<double,2,2> const& init_covar ){ this->init_covar_ = init_covar; }

	void set_min_covar( vnl_matrix_fixed<double,2,2> const& min_covar ){ this->min_covar_ = min_covar; }

private:
    graph_type graph_;

    bool change_;

    double max_prob_;

    graph_type::iterator curr_node_itr_;

    node_id_type curr_node_id_;

    time_type rel_time_curr_;

    time_type t_forget_;

    double prob_thresh_;

	double mahalan_dist_factor_;

    vnl_vector<double> curr_obs_;

    dbcl_state_machine_frame_clock_sptr classifier_clk_sptr_;

    node_id_type node_id_counter_;

    unsigned classifier_id_;

    graph_type::iterator add_node();

    bool remove_node( node_id_type const& node_id );

    void prune_graph();

    vnl_matrix_fixed<double,2,2> init_covar_;

    vnl_matrix_fixed<double,2,2> min_covar_;

    vcl_map<unsigned, bool> frame_change_map_;

	frame_state_map_type frame_training_state_map_;

	frame_state_map_type frame_test_state_map_;

	frame_mean_map_type frame_mean_map_;

	frame_covar_map_type frame_covar_map_;

	frame_transition_table_map_type frame_transition_table_map_;

	frame_mixture_weight_map_type frame_mixture_weight_map_;

	frame_max_prob_map_type frame_max_prob_map_;

	

	//vcl_map<unsigned, vcl_vector<vnl_vector_fixed<double,2> > > frame_mean_map_;

	//vcl_map<unsigned, vcl_vector<vnl_matrix_fixed<double,2,2> > > frame_covar_map_;

	//vcl_map<unsigned, transition_table_type> frame_transition_table_map_;
};

#endif //DBCL_STATE_MACHINE_CLASSIFIER_H_