//this is /contrib/bm/dsm2/dsm2_observer_base.h
#ifndef DSM2_OBSERVER_BASE_H_
#define DSM2_OBSERVER_BASE_H_
//:
// \file
// \date January 12, 2011
// \author Brandon A. Mayer
//
// A base class for observer classes to be notified
// by classes of dsm2_observable_base
//
// \verbatim
//  Modifications
// \endverbatim
#include<vbl/vbl_ref_count.h>

#include<vsl/vsl_binary_io.h>

//: Rorward declare so pointer can be used as argument 
//: to update.
class dsm2_subject_base;

class dsm2_observer_base: public vbl_ref_count
{
public:
	virtual ~dsm2_observer_base() {}

	virtual void update( dsm2_subject_base* s, 
					vcl_string const& interest ) = 0;

	virtual void b_read( vsl_b_istream& is ) = 0;

	virtual void b_write( vsl_b_ostream& os ) const = 0;

	unsigned id(){return this->id_;}

protected:
	dsm2_observer_base():id_(next_id_++){}

	unsigned id_;

	static unsigned next_id_;
};

#endif //DSM2_OBSERVER_BASE_H_