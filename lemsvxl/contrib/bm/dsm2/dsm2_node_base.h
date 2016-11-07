// this is /contrib/bm/dsm2/dsm2_node_base.h
#ifndef DSM2_NODE_BASE_H_
#define DSM2_NODE_BASE_H_
//:
// \file
// \date January 12, 2011
// \author Brandon A. Mayer
//
// A base node class for our dynamic state machine.
//
// \verbatim
//  Modifications
// \endverbatim
#include<vbl/vbl_ref_count.h>

#include<vcl_cstddef.h>

#include<vsl/vsl_binary_io.h>

class dsm2_node_base: public vbl_ref_count
{
public:
	dsm2_node_base(){}

	virtual ~dsm2_node_base(){}

	//ACESSORS
	//inline unsigned id() const { return this->id_; }

	virtual unsigned model_dim(){ return unsigned(0); };

	//SETTERS
	//inline void set_id( unsigned const& id ) { this->id_ = id; }

	virtual void b_write( vsl_b_ostream& os ) const {};

	virtual void b_read( vsl_b_istream const& is ){};

//protected:
//	unsigned id_;
};
#endif //DSM2_NODE_BASE_H_