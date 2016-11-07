//this is /contrib/bm/dsm2/dsm2_node_gaussian.h
#ifndef DSM2_NODE_GAUSSIAN_H_
#define DSM2_NODE_GAUSSIAN_H_
//:
// \file
// \date January 12, 2011
// \author Brandon A. Mayer
//
// A base concrete class for gaussian nodes. 
// Uses curiously reoccuring pattern. The node inherits all
// methods and members of the templated class which should be a 
// bsta gaussian model.
//
// \verbatim
//  Modifications
// \endverbatim
#include<dsm2/dsm2_node_base.h>

#include<bsta/bsta_gaussian_full.h>
#include<bsta/bsta_gaussian_sphere.h>

template<class gaussT>
class dsm2_node_gaussian: public dsm2_node_base, public gaussT
{
public:
    dsm2_node_gaussian(){}

    virtual ~dsm2_node_gaussian(){}

    typedef gaussT model_type;

    typedef typename gaussT::math_type math_type;

    typedef typename gaussT::vector_type vector_type;

    typedef typename gaussT::covar_type covar_type;

    enum{dimension = gaussT::dimension};

    //ACCESSORS
    virtual unsigned model_dim(){ return dimension; }

    //BINARY I/O
    virtual void b_write( vsl_b_ostream& os ) const;

    virtual void b_read( vsl_b_istream& is );
};

#endif //DSM2_NODE_GAUSSIAN_FULL_H_
