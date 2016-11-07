//this is /contrib/bm/dsm/dsm_ncn.h
#ifndef DSM_NCN_H_
#define DSM_NCN_H_
//:
// \file
// \date January 12, 2011
// \author Brandon A. Mayer
//
// Class to manage and extract Non-Compact Neighborhoods
//
// \verbatim
//  Modifications
// \endverbatim
#include<bsta/bsta_histogram.h>
#include<bsta/bsta_joint_histogram.h>

#include<bxml/bxml_document.h>
#include<bxml/bxml_write.h>
#include<bxml/bxml_read.h>
#include<bxml/bxml_find.h>

#include"dsm_utilities.h"

#include<vbl/vbl_ref_count.h>

#include<vcl_iomanip.h>
#include<vcl_map.h>
#include<vcl_vector.h>
#include<vcl_set.h>
#include<vcl_utility.h>

#include<vgl/vgl_point_2d.h>
#include<vgl/io/vgl_io_point_2d.h>

#include<vidl/vidl_image_list_istream.h>
#include<vidl/vidl_convert.h>

#include<vil/vil_image_resource_sptr.h>
#include<vil/vil_image_view.h>
#include<vil/vil_convert.h>
#include<vil/vil_new.h>
#include<vil/io/vil_io_image_view.h>

#include<vnl/vnl_math.h>
#include<vnl/vnl_random.h>

#include<vsl/vsl_binary_io.h>

class dsm_ncn: public vbl_ref_count
{
public:
	dsm_ncn():video_valid_(false), targets_valid_(false), neighborhood_valid_(false),entropy_valid_(false),candidate_pivot_valid_(false),
				num_neighbors_(unsigned(10)), num_pivot_pixels_(unsigned(2000)), num_particles_(unsigned(10000)), video_glob_(""){}

	dsm_ncn( vcl_string const& video_glob, vcl_vector<vgl_point_2d<unsigned> > const& targets,
				unsigned const& num_neighbors = 10, unsigned const& num_pivot_pixels = 2000, unsigned const& num_particles = 10000 );

	dsm_ncn( vcl_string const& video_glob, vcl_string const& targets_xml_path, 
				unsigned const& num_neighbors = 10, unsigned const& num_pivot_pixels = 2000, unsigned const& num_particles = 10000 );
				
	~dsm_ncn(){}

	vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >, dsm_vgl_point_2d_coord_compare<unsigned>  > neighborhood();

	bool calculate_temporal_entropy( unsigned const& nbins = 16 );

	bool sample_pivot_pixels();

	void add_target( vgl_point_2d<unsigned> const& target );

	void set_video_glob( vcl_string const& video_glob );

	void set_num_neighbors( unsigned const& nn ){this->num_neighbors_ = nn;}

	bool set_num_particles( unsigned const& np );

	bool set_num_pivot_pixels( unsigned const& npp );

	bool build_ncn();

	void load_video( vcl_string const& filename );

	void set_parameters( unsigned const& num_neighbors = 10, unsigned const& num_pivot_pixels = 2000, unsigned const& num_particles = 10000)
	{ this->num_neighbors_ = num_neighbors; this->num_pivot_pixels_ = num_pivot_pixels; this->num_particles_ = num_particles; }

	//i/o
	void load_entropy_bin( vcl_string const& filename );
	bool save_entropy_bin( vcl_string const& filename );
	bool save_entropy_dat( vcl_string const& filename );

	bool write_neighborhood_mfile( vcl_string const& filename );

	bool write_neighborhood_xml( vcl_string const& filename );
	bool read_neighborhood_xml( vcl_string const& filename );

	bool write_neighborhood_txt( vcl_string const& filename );
	bool write_pivot_pixel_candidates_txt( vcl_string const& filename );

	bool parse_target_xml( vcl_string const& targets_xml_path );
	
	void b_read(vsl_b_istream &is);
	void b_write(vsl_b_ostream &os) const;
	//void vsl_b_read(vsl_b_istream& is, dsm_ncn* p);
	//void vsl_b_write(vsl_b_ostream& os, dsm_ncn const* &p);
	//void vsl_print_summary(vcl_ostream& os, const dsm_ncn* p);
	
	
private:

	friend void vsl_b_write(vsl_b_ostream& os, dsm_ncn const& ncn);
	friend void vsl_b_read(vsl_b_istream& is, dsm_ncn& ncn);
	friend void vsl_print_summary( vcl_ostream& os, const dsm_ncn &ncn );
	
	bool video_valid_;
	bool targets_valid_;
	bool neighborhood_valid_;
	bool entropy_valid_;
	bool candidate_pivot_valid_;

	unsigned num_pivot_pixels_;
	unsigned num_particles_;
	unsigned num_neighbors_;
	vcl_set<vgl_point_2d<unsigned>, dsm_vgl_point_2d_coord_compare<unsigned> > pivot_pixel_candidates_;
	vil_image_view<double> temporal_entropy_;
	vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >, dsm_vgl_point_2d_coord_compare<unsigned>  > neighborhood_;
	vidl_image_list_istream video_stream_;
	vcl_string targets_xml_path_;
	vcl_string video_glob_;

	//PROTECTED MEMBER FUNCTIONS
	void build_frame_map_( vidl_image_list_istream const& video_stream, vcl_map<unsigned, vil_image_resource_sptr>& img_seq );
	
	static bool binary_search_predicate_(double& i, double& j){ return i > j; }

	
};

//void vsl_b_read(vsl_b_istream &is, dsm_ncn &ncn);
//void vsl_b_write(vsl_b_ostream &os, dsm_ncn const& ncn);
//void vsl_b_read(vsl_b_istream &is, dsm_ncn* &p);
//void vsl_b_write(vsl_b_ostream &os, const dsm_ncn* p);
//void vsl_print_summary(vcl_ostream& os, const dsm_ncn *p);

#endif //DSM_NCN_H_

