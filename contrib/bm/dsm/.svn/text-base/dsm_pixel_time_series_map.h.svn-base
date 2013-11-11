//this is /contrib/bm/dsm/dsm_pixel_time_series_map.h
#ifndef DSM_PIXEL_TIME_SERIES_MAP_H_
#define DSM_PIXEL_TIME_SERIES_MAP_H_
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
#include"dsm_time_series_sptr.h"

#include"dsm_utilities.h"

#include<vbl/io/vbl_io_smart_ptr.h>

#include<vbl/vbl_ref_count.h>

#include<vcl_map.h>
#include<vcl_sstream.h>
#include<vcl_utility.h>

#include<vgl/io/vgl_io_point_2d.h>

#include<vul/vul_file.h>

class dsm_pixel_time_series_map: public vbl_ref_count
{
public:

	typedef vcl_map<vgl_point_2d<unsigned>, dsm_time_series_sptr, dsm_vgl_point_2d_coord_compare<unsigned> > pixel_time_series_map_type;

	dsm_pixel_time_series_map(unsigned const& feature_dim = 128):feature_dimension(feature_dim){}

	dsm_pixel_time_series_map(unsigned const& x, unsigned const& y, dsm_time_series_sptr time_series_sptr, unsigned const& feature_dim = 128);

	~dsm_pixel_time_series_map(){}

	bool reduce_dims( unsigned const& ndims2keep );

	//will replace entry if one exists at x and y
	void insert(unsigned const& x, unsigned const& y, dsm_time_series_sptr ts_sptr);
	
	void insert( vgl_point_2d<unsigned> const& pt, dsm_time_series_sptr ts_sptr);

	void insert( unsigned const& x, unsigned const& y, unsigned const& time, dsm_feature_sptr feature_sptr);

	void insert( vgl_point_2d<unsigned> const& pt, unsigned const& time, dsm_feature_sptr feature_sptr);

	pixel_time_series_map_type::iterator time_series( vgl_point_2d<unsigned> const& pt );

	pixel_time_series_map_type::iterator time_series( unsigned const& x, unsigned const& y);

	//provide a directory write a a single file for every time series associated with a given pixel.
	bool write_txt( vcl_string const& filename ) const;

	void b_write(vsl_b_ostream& os) const;

	void b_read(vsl_b_istream& is);

	pixel_time_series_map_type pixel_time_series_map;

	unsigned feature_dimension;
};


#endif //DSM_PIXEL_TIME_SERIES_MAP_H_