//this is /contrib/bm/ncn/ncn_factory.h
#ifndef ncn_factory_h_
#define ncn_factory_h_

#include<bsta/bsta_histogram.h>
#include<bsta/bsta_joint_histogram.h>

#include"ncn_neighborhood_sptr.h"

#include<vbl/vbl_array_1d.h>

#include<vcl_iomanip.h>
#include<vcl_string.h>
#include<vcl_utility.h>

#include<vidl/vidl_convert.h>
#include<vidl/vidl_image_list_istream.h>

#include<vil/vil_convert.h>
#include<vil/vil_image_view.h>
#include<vil/vil_resample_bicub.h>


#include<vnl/vnl_math.h>
#include<vnl/vnl_matrix.h>
#include<vnl/vnl_random.h>

#include<vsl/vsl_binary_io.h>

class ncn_factory
{
public:

	ncn_factory():num_pivot_pixels_(2000),n_particles_(10000){}
	ncn_factory(vcl_string const& video_list_glob);
	ncn_factory(vcl_string const& video_list_glob, target_pixel_type const& target_pixels,unsigned const& n_neighbors_);
	ncn_factory(vcl_string const& video_list_glob, vsl_b_ifstream& entropy_binary, target_pixel_type const& target_pixels, unsigned const& n_neighbors_);

	~ncn_factory(){}
	void build_neighborhood();

	ncn_neighborhood_sptr neighborhood_sptr(){return neighborhood_sptr_;}
	//vidl_image_list_istream video_stream(){return this->video_stream_;}
	vil_image_view<double> entropy_matrix(){return this->entropy_view_;}

	pivot_pixel_candidate_type sample_pivot_pixels();
	vil_image_view<double> calculate_temporal_entropy(vidl_image_list_istream& video_stream);
	vil_image_view<double> load_binary_temporal_entropy();
	unsigned find_bin(vbl_array_1d<double> const& cdf, double const& target);

	

private:
	unsigned num_pivot_pixels_;
	unsigned n_particles_;
	vidl_image_list_istream video_stream_;
	vil_image_view<double> entropy_view_;
	ncn_neighborhood_sptr neighborhood_sptr_;

};
#endif //ncn_factory_h_