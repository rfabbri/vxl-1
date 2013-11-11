//this is /contrib/bm/dsm/dsm_manager.h
#ifndef DSM_MANAGER_H_
#define DSM_MANAGER_H_
//:
// \file
// \date March 27, 2011
// \author Brandon A. Mayer
//
// A Templated class for a state machine manager. This class will associate target locations with
// a state machine base smart pointer but will have read and write capability for a state_machine<T>.
// We have a base class because the manager class itself must be templated over the dimensionality of the 
// feature vector but we can define smart pointers to the base class for insertion into the python database.
//
// \verbatim
//  Modifications
// \endverbatim

#include"dsm_manager_base_sptr.h"
#include"dsm_state_machine.h"
#include"io/dsm_io_state_machine.h"
#include<vsl/vsl_binary_io.h>
template<unsigned T>
class dsm_manager: public dsm_manager_base
{
public:
    dsm_manager(){}

    ~dsm_manager(){}

    virtual unsigned ndims() const {return T;} 

    virtual void b_write(vsl_b_ostream& os) const;

    virtual void b_read(vsl_b_istream& is);
};

#endif //DSM_MANAGER_H_
