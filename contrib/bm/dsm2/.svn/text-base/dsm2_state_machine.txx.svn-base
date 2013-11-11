//this is /contrib/bm/dsm2/dsm2_state_machine.txx
#ifndef DSM2_STATE_MACHINE_TXX_
#define DSM2_STATE_MACHINE_TXX_
#include<dsm2/dsm2_state_machine.h>
#include<dsm2/io/dsm2_io_node_gaussian.h>
#include<dsm2/io/dsm2_io_transition_map.h>

template<class nodeT, class transitionT>
void dsm2_state_machine<nodeT, transitionT>::b_write( vsl_b_ostream& os ) const
{
	const short version_no = 1;
	vsl_b_write(os, version_no);

	//write the number of nodes
	vsl_b_write(os, this->graph.size());

	vcl_map<unsigned, dsm2_node_base_sptr>::const_iterator
		g_itr, g_end = this->graph.end();

	for( g_itr = this->graph.begin(); g_itr != g_end;
			++g_itr )
	{
		//write the node id
		vsl_b_write(os, g_itr->first);

		//write the node as a pointer
		vsl_b_write(os, static_cast<nodeT*>(g_itr->second.as_pointer()));
	}//end graph iteration

	////write the transition table
	//vsl_b_write(os, 
	//	static_cast<transitionT*>(this->transition_table_base_sptr.as_pointer()));

	return;
}//end b_write

template<class nodeT, class transitionT>
void dsm2_state_machine<nodeT, transitionT>::b_read( vsl_b_istream& is ) 
{
	short v;
	vsl_b_read(is, v);

	switch(v)
	{
	case 1:
		{
			//read number of nodes
			vcl_size_t nnodes;
			vsl_b_read(is, nnodes);

			for( vcl_size_t n = 0; n < nnodes; ++n )
			{
				//read the id
				unsigned id;
				vsl_b_read(is, id);

				//read the node
				nodeT* node_ptr;
				vsl_b_read(is, node_ptr);

				dsm2_node_base_sptr node_base_sptr(node_ptr);
				this->graph[id] = node_base_sptr;
			}

			//read the transition table
			transitionT* transition_ptr;
			vsl_b_read(is, transition_ptr);

			//this->transition_table_base_sptr = transition_ptr;
			break;
		}//end case 1
	default:
		{
			vcl_cerr << "ERROR: dsm2_state_machine::b_read() -- unknown version number." << vcl_flush;
			return;
		}//end default
	}//end switch(v)

	return;
}//end b_read

template<class nodeT, class transitionT>
dsm2_state_machine<nodeT, transitionT>::graph_type::iterator
	dsm2_state_machine<nodeT, transitionT>::add_node()
{
	dsm2_node_base_sptr node_sptr = new nodeT();

	vcl_pair<graph_type::iterator,bool> ret;

	ret = this->graph.insert(vcl_pair<unsigned, dsm2_node_base_sptr>(
								this->next_node_id++,node_sptr) );

	

	

	return ret.first;
}//end add_node()

//variadic macro as the template parameter will be another templated
//type we must not use the symantic parsing of the preprocessor
//and workaround by defining a variable length argument.
#define DSM2_STATE_MACHINE_INSTANTIATE(...)\
template class dsm2_state_machine< __VA_ARGS__ > 

#endif //DSM2_STATE_MACHINE_TXX_