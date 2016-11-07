//this is /contrib/bm/dsm/dsm_feature.h
#ifndef DSM_FEATURE_H_
#define DSM_FEATURE_H_
//:
// \file
// \date January 12, 2011
// \author Brandon A. Mayer
//
// A simple class to store a (feature, time) pair. This could have been implemented with
// standard containers but this class will serve as an interface to python.
//
// \verbatim
//  Modifications
// \endverbatim
#include<vbl/vbl_ref_count.h>

#include<vnl/vnl_vector.h>

#include<vnl/io/vnl_io_vector.h>

#include<vsl/vsl_binary_io.h>

class dsm_feature: public vbl_ref_count
{
public:
    dsm_feature(){}

    dsm_feature( vnl_vector<double> const& feature_vector, unsigned const& time):v(feature_vector), t(time){}

    virtual ~dsm_feature(){}

    vnl_vector<double> v;

	unsigned feature_dim() const { return v.size(); };

    unsigned t;

    void b_write(vsl_b_ostream& os) const;
   
    void b_read(vsl_b_istream& is);

};
#endif //DSM_FEATURE_BASE_H_