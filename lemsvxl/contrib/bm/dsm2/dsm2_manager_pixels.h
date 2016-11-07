//this is /contib/bm/dsm2/dsm2_manager_pixels.h
#ifndef DSM2_MANAGER_PIXELS_H_
#define DSM2_MANAGER_PIXELS_H_
//:
// \file
// \date March 9, 2011
// \author Brandon A. Mayer
//
// Concrete class for the state machine manager. Defines a relation
// between pixel coordinates and state machines
//
// \verbatim
//  Modifications
// \endverbatim
#include<dsm2/dsm2_manager_base.h>
#include<dsm2/dsm2_state_machine_base_sptr.h>
#include<dsm2/dsm2_vgl_point_2d_coord_compare.h>

#include<vcl_map.h>

#include<vgl/vgl_point_2d.h>

template<class pixelT, class smT>
class dsm2_manager_pixels:public dsm2_manager_base
{
public:
	dsm2_manager_pixels() {}

	virtual ~dsm2_manager_pixels(){}

	virtual void b_write( vsl_b_ostream& is ) const;

	virtual void b_read( vsl_b_istream& is );

	vcl_map<vgl_point_2d<pixelT>, dsm2_state_machine_base_sptr,
		dsm_vgl_point_2d_coord_compare<pixelT> > pixel_state_machine_map;
};

#endif //DSM2_MANAGER_PIXELS_H_