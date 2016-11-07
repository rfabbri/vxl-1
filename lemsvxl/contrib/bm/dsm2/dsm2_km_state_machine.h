//this is /contrib/bm/dsm2/dsm2_km_state_machine.h
#ifndef DSM2_KM_STATE_MACHINE_H_
#define DSM2_KM_STATE_MACHINE_H_

#include<dsm2/dsm2_node_gaussian.h>
#include<dsm2/dsm2_num_obs.h>
#include<dsm2/dsm2_state_machine_base.h>
#include<dsm2/dsm2_transition_map.h>

template<class nodeT>
class dsm2_km_state_machine: public dsm2_state_machine_base
{
public:
    //: relation node id - node id - time - frequency
    typedef dsm2_transition_map<unsigned,unsigned,unsigned>
        transition_table_class;

    typedef dsm2_transition_map<unsigned,unsigned,unsigned>::
        transition_table_type transition_table_type;

    typedef dsm2_num_obs<nodeT> node_type;

    typedef typename nodeT::model_type model_type;
    
    typedef typename nodeT::math_type math_type;

    typedef typename nodeT::vector_type vector_type;

    typedef typename nodeT::covar_type matrix_type;

    enum{dimension = nodeT::dimension};

    dsm2_km_state_machine():rel_time_curr(unsigned(0))
        { curr_node_itr = this->graph.end(); }

    virtual ~dsm2_km_state_machine(){}

    double transition_probability(unsigned const& node1, 
                                  unsigned const& node2,
                                  unsigned const& rel_time);

    void inc_transition_freq( unsigned const& node1,
                              unsigned const& node2,
                              unsigned const& rel_time);

    virtual graph_type::iterator add_node();

    virtual bool remove_node( unsigned const& node_id );

    virtual void b_write( vsl_b_ostream& os ) const;

    virtual void b_read( vsl_b_istream& is );

    transition_table_class transition_map;

    unsigned rel_time_curr;
};

#endif //DSM2_KM_STATE_MACHINE_H_
