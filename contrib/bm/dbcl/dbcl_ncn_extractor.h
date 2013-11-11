//this is contrib/bm/dbcl/dbcl_ncn_extractor.h
#ifndef DBCL_NCN_EXTRACTOR_H_
#define DBCL_NCN_EXTRACTOR_H_
//:
// \file
// \date January 3, 2011
// \author Brandon A. Mayer
//
// This is a replacement for the dncn_factory (in a separate library). Its purpose is to extract non compact neighborhoods
// given a list of target points.
//
// \verbatim
//  Modifications
// \endverbatim
#include<bsta/bsta_histogram.h>
#include<bsta/bsta_joint_histogram.h>

#include"dbcl_vgl_point_2d_dist_compare.h"

#include<vbl/vbl_ref_count.h>

#include<vcl_algorithm.h>
#include<vcl_iomanip.h>
#include<vcl_set.h>
#include<vcl_string.h>
#include<vcl_vector.h>

#include<vgl/vgl_point_2d.h>

#include<vidl/vidl_image_list_istream.h>
#include<vidl/vidl_convert.h>

#include<vil/vil_convert.h>
#include<vil/vil_image_view.h>
#include<vil/vil_image_resource_sptr.h>
#include<vil/io/vil_io_image_view.h>
#include<vil/vil_save.h>
#include<vil/vil_new.h>

#include<vnl/vnl_math.h>
#include<vnl/vnl_random.h>

class dbcl_ncn_extractor: public vbl_ref_count
{
public:
	typedef vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >, dbcl_vgl_point_2d_dist_compare > target_ncn_map_type;

	typedef vcl_set<vgl_point_2d<unsigned>, dbcl_vgl_point_2d_dist_compare > pivot_pixel_set_type;

	dbcl_ncn_extractor():neighborhood_valid_(false), entropy_valid_(false), pivot_set_valid_(false){}

	dbcl_ncn_extractor( vcl_string const& video_glob, vcl_vector<vgl_point_2d<unsigned> > const& target_points, 
						unsigned const& num_pivot_pixels = 2000, unsigned const& num_particles = 10000, unsigned const& num_neighbors = 10 );

	~dbcl_ncn_extractor(){}

	bool load_video(vcl_string const& video_glob);

	void calculate_temporal_entropy( unsigned const& nbins = 16 );

	bool sample_pivot_pixels();

	bool extract_neighbors();

	bool build_ncn_no_entropy();

	bool build_ncn();

	target_ncn_map_type non_compact_neighborhood(){return this->target_ncn_map_;}
	

	//i/o
	void load_entropy_bin(vcl_string const& filename);
	bool save_entropy_dat(vcl_string const& filename);
	bool save_entropy_bin(vcl_string const& filename);
	bool write_neighborhood_mfile(vcl_string const& filename);


	
private:
	bool neighborhood_valid_;
	bool entropy_valid_;
	bool pivot_set_valid_;

	unsigned num_pivot_pixels_;
	unsigned num_particles_;
	unsigned num_neighbors_;

	pivot_pixel_set_type pivot_pixel_set_;

	vil_image_view<double> temporal_entropy_;

	target_ncn_map_type target_ncn_map_;

	vcl_map<unsigned, vil_image_resource_sptr> img_seq_;

	//PROTECTED MEMBER FUNCTIONS
	void build_frame_map(vcl_string const& filename);

	static bool binary_search_predicate(double& i, double& j){return i > j;}

};



#endif //DBCL_NCN_EXTRACTOR_H_