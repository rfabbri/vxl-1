//this is /contrib/bm/dsm/dsm_state_machine_base.h
#ifndef DSM_STATE_MACHINE_BASE_H_
#define DSM_STATE_MACHINE_BASE_H_

#include"dsm_frame_clock.h"
#include"dsm_feature_sptr.h"
#include"dsm_node_base_sptr.h"

#include<vbl/vbl_ref_count.h>

#include<vcl_map.h>
#include<vcl_set.h>
#include<vcl_cmath.h>

#include<vnl/vnl_diag_matrix.h>
#include<vnl/vnl_matrix.h>
#include<vnl/vnl_vector.h>
#include<vnl/io/vnl_io_vector.h>

#include<vsl/vsl_binary_io.h>
#include<vsl/vsl_map_io.h>
#include<vsl/vsl_set_io.h>


class dsm_state_machine_base:public vbl_ref_count
{
public:
	//dsm_state_machine_base(){}

	dsm_state_machine_base(	unsigned const& t_forget = 30,
							double const& prob_thresh = 0.0002,
							double const& mahalan_dist_factor = 1,
							vnl_matrix<double> const& init_covar = vnl_diag_matrix<double>(2,1.0),
							vnl_matrix<double> const& min_covar = vnl_matrix<double>(2,2,0.0001) ):
		t_forget_(t_forget), prob_thresh_(prob_thresh), mahalan_dist_factor_(mahalan_dist_factor), 
			change_(false), next_node_id_(0),frame_clock_ptr_(dsm_frame_clock::instance()){ this->curr_node_itr_ = this->graph_.end();}

	virtual bool update_manhalanobis_distance( vnl_vector<double> const& obs) = 0;

	virtual bool classify( vnl_vector<double> const& obs ) = 0;

	virtual bool classify( dsm_feature_sptr feature_sptr ) = 0;

	virtual ~dsm_state_machine_base(){}

	virtual int ndims() = 0;

	virtual void b_write(vsl_b_ostream& os) const = 0; 

	virtual void b_read(vsl_b_istream& is) = 0;

	virtual void write_dot_file( vcl_string const& filename ) = 0;

	virtual void write_dot_file_full( vcl_string const& filename ) = 0;

	unsigned classifier_id() const { return this->classifier_id_; }

	bool change() const { return this->change_; }

	bool change( unsigned const& time ) { return this->frame_change_map_[time]; }

	unsigned size() const { return this->graph_.size(); }
	
	unsigned rel_time_curr() const { return this->rel_time_curr_; }

	unsigned curr_node_id() const { return this->curr_node_id_; }

	double max_prob() const { return this->max_prob_; }

	double mahalan_dist_factor() const { return this->mahalan_dist_factor_; }

	vcl_map<unsigned, dsm_node_base_sptr> graph() const { return this->graph_; }

	vcl_map<unsigned, dsm_node_base_sptr>::iterator curr_node_itr() const { return this->curr_node_itr_; }

	vcl_map<unsigned, bool> frame_change_map() const { return this->frame_change_map_; }

	vcl_map<unsigned, vcl_map<unsigned, vnl_vector<double> > > frame_mean_map() const { return this->frame_mean_map_; }

	vcl_map<unsigned, vcl_map<unsigned, vnl_matrix<double> > > frame_covar_map() const { return this->frame_covar_map_; }

	vcl_map<unsigned, vcl_map<unsigned, vcl_map<unsigned, unsigned> > > frame_transition_table_map() const { return this->frame_transition_table_map_; }

	vcl_map<unsigned, double> frame_max_prob_map() const { return this->frame_max_prob_map_; }

	vcl_map<unsigned, unsigned> frame_state_map() const { return this->frame_state_map_; }

	vcl_map<unsigned, vcl_map<unsigned, double> > frame_mixture_weight_map() const { return this->frame_mixture_weight_map_; }

	vcl_map<unsigned, dsm_node_base_sptr>::const_iterator graph_begin(){return this->graph_.begin();}

	vcl_map<unsigned, dsm_node_base_sptr>::const_iterator graph_end(){return this->graph_.end();}

	vcl_map<unsigned,bool> frame_change_map_;

	vcl_map<unsigned, vcl_map<unsigned, vnl_vector<double> > > frame_mean_map_;

	vcl_map<unsigned, vcl_map<unsigned, vnl_matrix<double> > > frame_covar_map_;

	vcl_map<unsigned, vcl_map< unsigned, vcl_map<unsigned, unsigned> > > frame_transition_table_map_;

	vcl_map<unsigned, double> frame_max_prob_map_;

	//map a frame to a node id
	vcl_map<unsigned, unsigned> frame_state_map_;

	// this has the form vcl_map<time, vcl_map<node_id, mixture weight> >
	vcl_map<unsigned, vcl_map< unsigned, double> >frame_mixture_weight_map_;

protected:


	unsigned classifier_id_;

	unsigned next_node_id_;

	dsm_frame_clock* frame_clock_ptr_;

	//graph is a map between node ids and node pointers
	vcl_map<unsigned, dsm_node_base_sptr> graph_;
	
	//an iterator which points to the current node.
	vcl_map<unsigned,dsm_node_base_sptr>::iterator curr_node_itr_;

	unsigned curr_node_id_;

	unsigned rel_time_curr_;

	unsigned t_forget_;

	double mahalan_dist_factor_;

	vnl_vector<double> curr_obs_;

	bool change_;

	double max_prob_;

	double prob_thresh_;

	//vnl_matrix<double> init_covar_;

	//vnl_matrix<double> min_covar_;

	//A set of node id's the target has visited.
	vcl_set<unsigned> target_visited_;

	//Protected MEMBER FUNCTIONS
	virtual vcl_map<unsigned,dsm_node_base_sptr>::iterator add_node() = 0;

	virtual bool remove_node( unsigned const& node_id ) = 0;

	virtual void prune_graph() = 0;

};



#endif //DSM_STATE_MACHINE_BASE_H_