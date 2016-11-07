// This is contrib/bm/ncn_utilities.h
#ifndef ncn_utilities_h_
#define ncn_utilities_h_
//
// \file
// \author Brandon A. Mayer
// \local utility functions for non compact neighborhood
// \verbatim
//  Modifications
//  May 24 2010 Initial Version
// \endverbatim
//

#include< bsta/bsta_histogram.h >
#include< bsta/bsta_joint_histogram.h >
#include< bsta/bsta_sampler.h >

#include "ncn_mutual_information.h"
#include "ncn_image_point.h"

#include< vcl_algorithm.h >
#include< vcl_iomanip.h >
#include< vcl_iostream.h >

#include< vcl_map.h>
#include< vcl_set.h>
#include< vcl_utility.h >
#include< vcl_string.h >

#include<vgl/vgl_point_2d.h>

#include< vil/vil_convert.h >
#include< vil/vil_image_view.h >
#include< vil/vil_load.h >
#include< vil/vil_save.h >

#include < vnl/vnl_vector.h >
#include < vnl/vnl_math.h >
#include < vnl/vnl_matrix.h >
#include < vnl/vnl_random.h > 
#include < vnl/io/vnl_io_matrix.h >

#include< vsl/vsl_binary_io.h >

#include < vul/vul_file.h >
#include < vul/vul_file_iterator.h >

class ncn_utilities
{
public:
    ~ncn_utilities() {}
    
    static vcl_map<unsigned, vil_image_view<float> > load_image_sequence(vcl_string const& img_directory);

    //produce .dat file that can be loaded into matlab
    static void vnl_matrix2dat(vcl_ofstream& ofs,vnl_matrix<float> const& mat);

    //produce .dat file from unsigned matrix
    static void vnl_matrix2dat(vcl_ofstream& ofs,vnl_matrix<unsigned> const& mat);

    //produce .dat file from vnl_vector unsigned
    static void vnl_vector2dat(vcl_ofstream& ofs, vnl_vector<float> const& vec);

    //calculate the entropy of each pixel over time
    static vnl_matrix<float> calculate_temporal_entropy(vcl_map<unsigned, vil_image_view<float> >& img_sequence, unsigned const& nbins);

    //normalize entropy matrix
    static bool normalize_entropy_matrix(vnl_matrix<float>& mat);

    //sample pivot pixels from entropy matrix
    //will return a num_piv_pix x 2 indicating the (x,y) values of each pivot pixel.
    static bool sample_pivot_pixels(vnl_matrix<float> const& entropy_matrix,unsigned const& num_piv_pix,
        vnl_matrix<unsigned>& output);

    //sample pivot pixels from entropy matrix
    //will first down sample the entropy matrix (effective pixel locations)
    //will return a num_piv_pix x 2 indicating the (x,y) values of each pivot pixel.
    static bool sample_pivot_pixels_fast(vnl_matrix<float> const& entropy_matrix, unsigned const& num_piv_pix,
            unsigned const& down_sample_factor, vnl_matrix<unsigned>& output);

    // We will use the inverse sampling method to draw a set of samples and reject those samples
    //that are not unique. This is not exactly sampling without replacement but close enough as the events we are trying to avoid;
    //replecation of sample points is extremely unlikely given the sample space is much larger than the required number of samples.
    //Down sampling is still an option with this method. If no downsampling is required, down_sample_factor = 1;
    //Also implments binary search algorithm (divide and conquer) to locate correct bin.
    static bool sample_pivot_pixels_rejection(vnl_matrix<float> const& entropy_matrix, unsigned const& num_piv_pix,
             vnl_matrix<unsigned>& output,unsigned const& down_sample_factor = 1);

    //importance sampling combined with the non-unique rejection techniques. Downsampling is an implicit characteristic
    //of this method.
    static bool sample_pivot_pixels_importance(vnl_matrix<float> const& entropy_matrix, unsigned const& num_piv_pix,
             vnl_matrix<unsigned>& output, unsigned const& nparticles = 5000);   

    static bool sample_pivot_pixels_importance(vnl_matrix<float> const& entropy_matrix, unsigned const& num_samples,
             vcl_set<ncn_image_point>& pivot_pixel_candidates, unsigned const& nparticles = 5000);

    //sample the whole thing just use the binary search algorithm for speed up.
    static bool sample_pivot_pixels_dc(vnl_matrix<float> const& entropy_matrix, unsigned const& num_piv_pix,
             vnl_matrix<unsigned>& output);

    //returns index of bin
    static unsigned find_bin(vcl_vector<float> const& cdf, float const& target);

    //extract region of interest in the image sequence
    static bool get_region(unsigned const& x_tl, unsigned const& y_tl, unsigned const& x_lr, unsigned const& y_lr, vcl_vector<ncn_image_point>& roi);

    static bool get_region(unsigned const& x_tl, unsigned const& y_tl, unsigned const& x_lr, unsigned const& y_lr, vcl_vector<vgl_point_2d<unsigned> >&roi);

    //form non-compact neighborhood for region of interest
    static bool get_neighborhood(unsigned x_tl,unsigned y_tl, unsigned x_lr, unsigned y_lr, vcl_map<unsigned,vil_image_view<float> >& img_seq,
                                    unsigned const& num_neighbors, vcl_set<ncn_image_point> pivot_pixel_candidates,
                                    vcl_map<vcl_vector<ncn_image_point>::const_iterator, vcl_set<ncn_image_point> >non_compact_neighborhood);

    
    static bool pointSet2dat(vcl_ostream& os, vcl_set<ncn_image_point> const& point_set);

    static bool pointVect2dat(vcl_ostream& os, vcl_vector<ncn_image_point> const& point_vector);

    static bool pointVect2dat(vcl_ostream& os, vcl_vector<vgl_point_2d<unsigned> > const& point_vector);
};

#endif //ncn_utilities_h_