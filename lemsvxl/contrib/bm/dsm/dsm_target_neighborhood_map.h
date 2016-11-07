//this is /contrib/bm/dsm/dsm_pixel_time_series_map.h
#ifndef DSM_TARGET_NEIGHBORHOOD_MAP_H_
#define DSM_TARGET_NEIGHBORHOOD_MAP_H_
//:
// \file
// \date January 12, 2011
// \author Brandon A. Mayer
//
// A simple class to store a (target,neighborhood) pair. This could have been implemented with
// standard containers but this class will serve as an interface to python.
//
// \verbatim
//  Modifications
// \endverbatim
#include"dsm_utilities.h"

#include<vcl_map.h>

#include<vbl/vbl_ref_count.h>

#include<vgl/vgl_point_2d.h>
#include<vgl/io/vgl_io_point_2d.h>

#include<vsl/vsl_binary_io.h>

class dsm_target_neighborhood_map: public vbl_ref_count
{
public:

	typedef vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >, dsm_vgl_point_2d_coord_compare<unsigned> > 
		dsm_target_neighborhood_map_type;

	dsm_target_neighborhood_map(){}

	~dsm_target_neighborhood_map(){}

	dsm_target_neighborhood_map( vgl_point_2d<unsigned> const& target, vcl_vector<vgl_point_2d<unsigned> > const& neighborhood )
	{ this->target_neighborhood_map_[target] = neighborhood; }

	unsigned num_neighbors() const;
	
	//mutators
	void insert(vgl_point_2d<unsigned> const& target, vcl_vector<vgl_point_2d<unsigned> > const& neighborhood);

	dsm_target_neighborhood_map_type::iterator neighborhood( vgl_point_2d<unsigned> const& target );

	dsm_target_neighborhood_map_type::iterator neighborhood( unsigned const& x, unsigned const& y);

	//i/o
	bool write_txt( vcl_string const& filename) const;

	void b_write(vsl_b_ostream& os) const;

	void b_read(vsl_b_istream& is);

	//the map
	dsm_target_neighborhood_map_type target_neighborhood_map_;
	
};

#endif //DSM_TARGET_NEIGHBORHOOD_MAP_H_