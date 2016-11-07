//this is /contrib/bm/dsm2/dsm2_state_machine_base.h
#ifndef DSM2_STATE_MACHINE_BASE_H_
#define DSM2_STATE_MACHINE_BASE_H_
#include<dsm2/dsm2_node_base_sptr.h>
#include<dsm2/dsm2_transition_table_base_sptr.h>
#include<dsm2/dsm2_subject_base.h>

#include<dsm2/dsm2_vgl_point_2d_coord_compare.h>

#include<vbl/vbl_ref_count.h>

#include<vcl_map.h>
#include<vnl/vnl_vector.h>

#include<vsl/vsl_binary_io.h>

class dsm2_state_machine_base: public vbl_ref_count, public dsm2_subject_base
{
public:
	dsm2_state_machine_base():next_node_id(unsigned(0)){}

	typedef vcl_map<unsigned, dsm2_node_base_sptr> graph_type;

	virtual ~dsm2_state_machine_base(){}

	virtual void b_write( vsl_b_ostream& os ) const = 0;

	virtual void b_read(vsl_b_istream& is ) = 0;

	virtual vcl_map<unsigned, dsm2_node_base_sptr>::iterator add_node() = 0;

	virtual bool remove_node( unsigned const& node_id ) = 0;

	graph_type graph;

	graph_type::iterator curr_node_itr;

	unsigned next_node_id;
};

#endif //DSM2_STATE_MACHINE_BASE_H_