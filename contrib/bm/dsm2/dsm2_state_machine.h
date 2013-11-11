//this is /contrib/bm/dsm2/dsm2_state_machine.h
#ifndef DSM2_STATE_MACHINE_H_
#define DSM2_STATE_MACHINE_H_
#include<dsm2/dsm2_state_machine_base.h>
#include<dsm2/dsm2_node_gaussian.h>
#include<dsm2/io/dsm2_io_node_gaussian.h>
#include<dsm2/dsm2_transition_map.h>
#include<dsm2/io/dsm2_io_transition_map.h>

#include<vbl/vbl_ref_count.h>

#include<vsl/vsl_binary_io.h>

template<class nodeT, class transitionT>
class dsm2_state_machine: public dsm2_state_machine_base
{
public:
	typedef nodeT node_class_type;

	typedef typename nodeT::math_type math_type;

	typedef typename nodeT::covar_type covar_type;

	typedef typename nodeT::vector_type vector_type;

	enum{ dimension = nodeT::dimension};

	typedef transitionT transition_class_type;

	typedef typename transitionT::node_id_type node_id_type;

	typedef typename transitionT::time_type time_type;

	typedef typename transitionT::transition_table_type transition_table_type;

	dsm2_state_machine(){}

	~dsm2_state_machine(){}

	virtual graph_type::iterator add_node();

	virtual bool remove_node( unsigned const& node_id ){ return false; }

	virtual void b_write( vsl_b_ostream& os ) const;

	virtual void b_read( vsl_b_istream& is );

};

#endif //DSM2_STATE_MACHINE_H_