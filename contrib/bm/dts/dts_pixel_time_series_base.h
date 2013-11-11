//this is /contrib/bm/dts/dts_pixel_time_series_base.h
#ifndef DTS_PIXEL_TIME_SERIES_BASE_H_
#define DTS_PIXEL_TIME_SERIES_BASE_H_
//:
// \file
// \date October 9, 2011
// \author Brandon A. Mayer
//
// Base class for mapping between pixels and time series
//
// \verbatim
//  Modifications
// \endverbatim

#include<vbl/vbl_ref_count.h>

#include<vcl_cstddef.h>

#include<vgl/vgl_point_2d.h>

#include<vsl/vsl_binary_io.h>

class dts_pixel_time_series_base: public vbl_ref_count
{
public:
    dts_pixel_time_series_base(){}

    virtual ~dts_pixel_time_series_base(){}

    virtual vcl_size_t size() const = 0;

    virtual unsigned dimension() = 0;

    virtual void b_write( vsl_b_ostream& os ) const = 0;

    virtual void b_read( vsl_b_istream& is ) = 0;

	virtual bool write_txt( vcl_string const& filename,
								vgl_point_2d<unsigned> const& pt )= 0;
};

#endif //DTS_PIXEL_TIME_SERIES_BASE_H_