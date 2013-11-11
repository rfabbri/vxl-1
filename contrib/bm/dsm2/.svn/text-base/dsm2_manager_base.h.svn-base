//this is /contrib/bm/dsm2/dsm2_manager_base.h
#ifndef DSM2_MANAGER_BASE_H_
#define DSM2_MANAGER_BASE_H_
//:
// \file
// \date March 9, 2011
// \author Brandon A. Mayer
//
// Base class for a state machine manager. The concrete
// implmentation will define a relationship of ids to 
// state machines.
//
// \verbatim
//  Modifications
// \endverbatim
#include<dsm2/dsm2_state_machine_base_sptr.h>

#include<vbl/vbl_ref_count.h>

#include<vcl_map.h>

#include<vsl/vsl_binary_io.h>

class dsm2_manager_base: public vbl_ref_count
{
public:
	dsm2_manager_base(){}

	virtual ~dsm2_manager_base(){}

    virtual void b_write( vsl_b_ostream& os ) const = 0;

    virtual void b_read( vsl_b_istream& is ) = 0;
};


#endif //DSM2_MANAGER_BASE_H_