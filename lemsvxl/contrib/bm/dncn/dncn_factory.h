//this is contrib/bm/dncn/dncn_factory.h
#ifndef dncn_factory_h_
#define dncn_factory_h_
//:
// \file
// \date August 12, 2010
// \author Brandon A. Mayer
//
// Factory class to create dncn_target_list_2d. The interface to this class is to define a vcl_vector< dncn_image_point_2d > 
// and pass to the factory class wheras this will produce a target list will appropriate neighborhoods, feature vectors and 
// labels for classification regarding if the point is a ground truth label etc.
//
// \verbatim
//  1/3/2011 Brandon A. Mayer:
//		After implementing dense sift in bseg/bapl/bapl_dense_sift.h added functionality to extract sift features at each
//		target and neighbor in the function dncn_factory::extract_sift_features()
// \endverbatim
#include"bseg/bapl/bapl_dense_sift_sptr.h"


#include<bsta/bsta_histogram.h>
#include<bsta/bsta_joint_histogram.h>

#include"dncn_less_than.h"
#include"dncn_target_2d_sptr.h"
#include"dncn_target_list_2d_sptr.h"
#include"dncn_feature_sptr.h"
#include"dncn_neighborhood_2d_sptr.h"
#include"dncn_image_point_2d.h"

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
#include<vil/io/vil_io_image_view.h>
#include<vil/vil_save.h>
#include<vil/vil_new.h>

#include<vnl/vnl_math.h>
#include<vnl/vnl_random.h>

//bool binary_search_predicate(double i, double j){ return i > j; } 

class dncn_factory
{
public:

    typedef vcl_vector<dncn_image_point_2d> point_vector_type;

    typedef vcl_set<vgl_point_2d<unsigned>, dncn_less_than> pivot_pixel_set_type;

    dncn_factory():num_pivot_pixels_(2000),num_particles_(10000),num_neighbors_(10){target_list_2d_sptr_ = new dncn_target_list_2d;}

    dncn_factory( vcl_string& video_glob, point_vector_type& pv, unsigned num_pivot_pixels = 2000, unsigned num_particles = 10000, unsigned num_neighbors = 10);

    ~dncn_factory(){}

    //PROCESSES

    //will recalculate temporal entropy
    bool build();
    bool build( point_vector_type point_vector );

    //build without calculating entropy
    bool build_no_entropy();
    bool build_no_entropy( point_vector_type point_vector );
	bool build_no_entropy_sift();

    void calculate_temporal_entropy();

    void sample_pivot_pixels();

    void extract_neighbors();

    void extract_features();

	void extract_sift_features( unsigned const& octave_size = 6, unsigned const& num_octaves = 1);

	void reduce_dimensionality( unsigned const& dimensions_to_retain = 2 );

    void build_target_list();
  
    //setters
    void set_num_pivot_pixels( unsigned const& num_pivot_pixels ){ this->num_pivot_pixels_ = num_pivot_pixels; }

    void set_num_particles( unsigned const& num_particles ){ this->num_particles_ = num_particles; }

    void set_video_stream( vcl_string& video_glob ){ video_stream_.close(); video_stream_.open(video_glob); }

    void set_target_list_2d_sptr( dncn_target_list_2d_sptr tl ){ target_list_2d_sptr_ = tl; }

    //will replace the target in the list if the target exists.
    void add_target( dncn_target_2d_sptr target_sptr );

    //getters
    unsigned num_pivot_pixels(){ return num_pivot_pixels_; }

    unsigned num_particles(){ return num_particles_; }

    unsigned num_neighbors(){ return num_neighbors_; }

    //vidl_image_list_istream video_stream(){ return video_stream_; }

    dncn_target_list_2d_sptr target_list_2d_sptr(){ return target_list_2d_sptr_; }

    //i/o
    void save_entropy_bin( vcl_string const& filename );
    void load_entropy_bin( vcl_string const& filename );
    void save_entropy_dat( vcl_string const& filename );
    void save_pivot_pixels_mfile( vcl_string const& filename );
    void write_neighborhood_mfile( vcl_string const& filename );
    void write_feature_mfile( vcl_string const& filename ){this->target_list_2d_sptr_->write_feature_mfile(filename,this->video_stream_.num_frames());}
    void write_reduced_feature_mfile( vcl_string const& filename){ this->target_list_2d_sptr_->write_reduced_feature_mfile( filename,this->video_stream_.num_frames() );}

    void save_factory_bin( vcl_string const& filename );
    void read_factory_bin( vcl_string const& filename );

    static bool binary_search_predicate(double i, double j){ return i > j; } 

	//bapl_lowe_keypoint_sptr make_keypoint(vgl_point_2d<unsigned> const& location, bapl_lowe_pyramid_set_sptr pyramid_set);

private:
    //PROTECTED MEMBER VARIABLES:
    unsigned num_pivot_pixels_;
    unsigned num_particles_;
    unsigned num_neighbors_;
    vil_image_view<double> temporal_entropy_;

    vidl_image_list_istream video_stream_;
    dncn_target_list_2d_sptr target_list_2d_sptr_;

    vcl_vector<vgl_point_2d<unsigned> > target_list_;

    point_vector_type point_vector_;
    pivot_pixel_set_type pivot_pixel_set_;

    //PROTECTED MEMBER FUNCTIONS:
    vcl_map<unsigned, vil_image_view<vxl_byte> > build_frame_map();

    
};

#endif //dncn_factory_h_