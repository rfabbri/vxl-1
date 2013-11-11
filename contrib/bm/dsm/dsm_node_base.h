//this is /contrib/bm/dsm/dsm_node_base.h
#ifndef DSM_NODE_BASE_H_
#define DSM_NODE_BASE_H_

#include"dsm_frame_clock.h"

#include<vbl/vbl_ref_count.h>

#include<vcl_map.h>
#include<vcl_set.h>
#include<vcl_utility.h>

#include<vnl/vnl_vector.h>

#include<vsl/vsl_binary_io.h>

class dsm_node_base: public vbl_ref_count
{
public:
	dsm_node_base(){};

	dsm_node_base(unsigned const& node_id);

	virtual ~dsm_node_base(){}

	unsigned id(){ return this->id_; }

	unsigned nobs() const { return nobs_; }

	vcl_map<unsigned, vcl_map<unsigned, unsigned> > transition_table(){ return this->transition_table_; }

	bool inc_trans_freq( unsigned const& node_id, unsigned const& relative_time );

	double transition_prob( unsigned const& node_id, unsigned const& relative_time );

    //=========================================================================================================
    //virtual functions

    virtual double model_prob(vnl_vector<double> const& obs) = 0;
    
	virtual void init_model(vnl_vector<double> const& obs, vnl_matrix<double> const& init_covar) = 0;
 
    virtual void update_model( vnl_vector<double> const& obs, unsigned const& relative_time, vnl_matrix<double> const& min_covar ) = 0;
 
	virtual int model_dim() = 0;
 
	virtual void b_write(vsl_b_ostream& os) const = 0; 
 
    virtual void b_read(vsl_b_istream& is) = 0;

protected:
    //make friend for direct access of protected data members.
    template<int T>
    friend class dsm_state_machine;

	dsm_frame_clock* frame_clock_ptr_;

	unsigned id_;

	unsigned nobs_;

	//maps node ids to a time, frequency map.
	vcl_map<unsigned, vcl_map<unsigned, unsigned> > transition_table_;

	//a set of time stamps
	vcl_set<unsigned> relative_time_set_;

	//the last time this node was the current node
	unsigned last_time_curr_;

};

#endif //DSM_NODE_BASE_H_