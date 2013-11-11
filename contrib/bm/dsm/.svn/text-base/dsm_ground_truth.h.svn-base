//this is /contrib/bm/dsm/dsm_ground_truth.h
#ifndef DSM_GROUND_TRUTH_H_
#define DSM_GROUND_TRUTH_H_
//:
// \file
// \date August 5, 2011
// \author Brandon A. Mayer
// 
// A wrapper around the video ground truth.
//
// \verbatim
//  Modifications
// \endverbatim
#include<bsol/bsol_algs.h>

#include"dsm_utilities.h"

#include<vbl/vbl_ref_count.h>

#include<vcl_algorithm.h>
#include<vcl_iomanip.h>
#include<vcl_iostream.h>
#include<vcl_map.h>
#include<vcl_sstream.h>
#include<vcl_string.h>
#include<vcl_vector.h>
#include<vcl_utility.h>

#include<vgl/vgl_polygon.h>
#include<vgl/vgl_point_2d.h>
#include<vgl/vgl_polygon_scan_iterator.h>

#include<vidl/vidl_convert.h>
#include<vidl/vidl_image_list_istream.h>

#include<vil/vil_image_view.h>
#include<vil/vil_save.h>
#include<vil/io/vil_io_image_view.h>

#include<vsl/vsl_binary_io.h>

#include<vsol/vsol_polygon_2d.h>
#include<vsol/vsol_box_2d.h>
#include<vsol/vsol_polygon_2d_sptr.h>

#include<vul/vul_file.h>

class dsm_ground_truth: public vbl_ref_count
{
public:
	dsm_ground_truth(){}

	~dsm_ground_truth(){}

	//: relation: frame, polygon
	vcl_map<unsigned, vcl_vector<vsol_polygon_2d_sptr> > frame_polygon_map;

	//: relation: frame, change type
	vcl_map<unsigned, vcl_vector<vcl_string> > frame_change_type_map;

	//: build the change maps
	//: ni = image width
	//: nj = image height
	void build_change_maps(unsigned const& ni, unsigned const& nj);

	vcl_map<unsigned, vil_image_view<vxl_byte> >
		build_change_map_classical(unsigned const& ni, unsigned const& nj) const;

	void save_change_maps_classical_tiff( unsigned const& ni, 
						unsigned const& nj, vcl_string const& result_dir) const;

	void save_change_maps_tiff( vcl_string const& result_dir );

	//: relation: frame, change type, pixel locations
	vcl_map<unsigned, vil_image_view<vxl_byte> > change_maps;

	void b_read_bwm_gt( vcl_string const& filename );

	void b_write_bwm_gt( vcl_string const& filename ) const;

	void b_write(vsl_b_ostream& os) const;
   
    void b_read(vsl_b_istream& is);
};



#endif //DSM_GROUND_TRUTH_H_