//this is contrib/bm/ncn1_factory.h
#ifndef ncn1_factory_h_
#define ncn1_factory_h_

#include<bsta/bsta_histogram.h>
#include<bsta/bsta_joint_histogram.h>

#include"ncn1_neighborhood_sptr.h"
#include"ncn1_feature_sptr.h"

#include"ncn1_vgl_point_2d_less_than.h"

#include<vbl/vbl_array_1d.h>

#include<vcl_iomanip.h>
#include<vcl_map.h>
#include<vcl_ostream.h>
#include<vcl_set.h>
#include<vcl_utility.h>


#include<vidl/vidl_image_list_istream.h>
#include<vidl/vidl_convert.h>

#include<vil/vil_crop.h>
#include<vil/vil_convert.h>
#include<vil/vil_save.h>
#include<vil/vil_image_view.h>
#include<vil/io/vil_io_image_view.h>

#include<vgl/vgl_box_2d.h>
#include<vgl/vgl_point_2d.h>

#include<vnl/vnl_math.h>
#include<vnl/vnl_random.h>

class ncn1_factory
{
public:
	ncn1_factory():num_pivot_pixels_(2000),n_particles_(10000),n_neighbors_(10){neighborhood_sptr_ = new ncn1_neighborhood; feature_sptr_ = new ncn1_feature(10+1); neighborhood_sptr_->set_num_neighbors(this->n_neighbors_);}

	ncn1_factory(vidl_image_list_istream& video_stream, unsigned num_pivot_pixels = 2000, unsigned n_particles = 10000, unsigned n_neighbors = 10)
    {video_stream_ = video_stream; num_pivot_pixels_ = num_pivot_pixels; n_particles_ = n_particles; n_neighbors_ = n_neighbors; neighborhood_sptr_ = new ncn1_neighborhood; feature_sptr_ = new ncn1_feature(n_neighbors+1); neighborhood_sptr_->set_num_neighbors(this->n_neighbors_);}

    ncn1_factory(vcl_string const& video_glob, unsigned num_pivot_pixels = 2000, unsigned n_particles = 10000, unsigned n_neighbors = 10);
    
	~ncn1_factory(){}

    //processes to build the neighborhood
	unsigned find_bin(vbl_array_1d<double> const& cdf, double const& target);

    void calculate_temporal_entropy();

    void calculate_temporal_entropy(vgl_box_2d<unsigned> const& roi);

	void calculate_temporal_entropy(vidl_image_list_istream& video_stream);

	vil_image_view<double> calculate_temporal_entropy(vcl_map<unsigned, vil_image_view<unsigned> >& img_seq);

    pivot_pixel_list_type sample_pivot_pixel_candidates();

    void build_neighborhood();

    void extract_features();

    //Set and get functions
    void set_temporal_entropy(vil_image_view<double>& temporal_entropy){this->temporal_entropy_ = temporal_entropy;}

    void set_neighborhood( ncn1_neighborhood_sptr& nbrhd_sptr ){ neighborhood_sptr_ = nbrhd_sptr; }

    ncn1_neighborhood_sptr neighborhood_sptr(){ return neighborhood_sptr_; } 

    vil_image_view<double> temporal_entropy(){return this->temporal_entropy_;}

	void set_vid_stream(vidl_image_list_istream const& video_stream){video_stream_ = video_stream;}

    void set_target_pixel_list( target_pixel_list_type target_pixel_list ){ neighborhood_sptr_->set_target_pixel_list(target_pixel_list); }

    target_pixel_list_type target_pixel_list(){ return this->neighborhood_sptr_->target_pixel_list(); }

    void set_pivot_pixel_candidates( pivot_pixel_list_type& piv ) { this->neighborhood_sptr_->set_pivot_pixel_candidates(piv); }

    ncn1_feature_sptr feature_sptr(){ return feature_sptr_; }

    //Load and save functions
	void load_binary_temporal_entropy(vcl_string const& filename, vil_image_view<double>& temporal_entropy);

    void load_binary_temporal_entropy(vcl_string const& filename);

	void save_binary_temporal_entropy(vcl_string const& filename, vil_image_view<double> const& temporal_entropy);

    void save_binary_temporal_entropy(vcl_string const& filename);

    void save_matlab_temporal_entropy(vcl_string const& filename);

    void save_feature_dat( vcl_string const& filename ){ this->feature_sptr_->save_feature_dat(filename); }

private:
	unsigned num_pivot_pixels_;
	unsigned n_particles_;
	unsigned n_neighbors_;
	vidl_image_list_istream video_stream_;
	vil_image_view<double> temporal_entropy_;
    ncn1_neighborhood_sptr neighborhood_sptr_;
    ncn1_feature_sptr feature_sptr_;
	bool entropy_calculated_;
};

#endif //ncn1_factory_h_