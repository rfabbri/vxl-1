//this is /contrib/bm/dts/dts_time_series_fixed_base.h
#ifndef DTS_TIME_SERIES_FIXED_BASE_H_
#define DTS_TIME_SERIES_FIXED_BASE_H_
//:
// \file
// \date October 6, 2011
// \author Brandon A. Mayer
//
// A simple class to store a (time, vnl_vector_fixed) pair.
//
// \verbatim
//  Modifications
// \endverbatim
#include<vbl/vbl_ref_count.h>
#include<vbl/vbl_smart_ptr.h>

#include<vcl_cstddef.h>

#include<vsl/vsl_binary_io.h>

class dts_time_series_base: public vbl_ref_count
{
public:
    dts_time_series_base(){}

    virtual ~dts_time_series_base(){}

    virtual vcl_size_t size() const = 0;

    virtual unsigned dimension() = 0;

    virtual void b_write( vsl_b_ostream& os ) const = 0;

    virtual void b_read( vsl_b_istream& is ) = 0;

    virtual void write_txt( vcl_string const& filename) const = 0;
};


#endif //DTS_TIME_SERIES_FIXED_BASE_H_