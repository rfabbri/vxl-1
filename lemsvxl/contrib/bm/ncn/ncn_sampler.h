//this is contrib/bm/ncn_sample.h
#ifndef ncn_sampler_h_
#define ncn_sampler_h_
//
// \file
// \author Brandon A. Mayer
// \function to sample temporal entropy matrix
// \verbatim
// \Modifications
//  June 7 2010 Initial Version
// \endverbatim

#include<vbl/vbl_array_1d.h>

#include<vcl_iostream.h>
#include<vcl_iomanip.h>
#include<vcl_map.h>
#include<vcl_set.h>
#include<vcl_utility.h>

#include<vgl/vgl_distance.h>
#include<vgl/vgl_point_2d.h>

#include<vnl/vnl_math.h>
#include<vnl/vnl_matrix.h>
#include<vnl/vnl_random.h>
#include<vnl/vnl_vector.h>

class vgl_point_2d_less_than
{
public:
    vgl_point_2d_less_than(vgl_point_2d<unsigned> const& p): focus_(p){}
    vgl_point_2d_less_than(){vgl_point_2d<unsigned> temp(0,0); focus_ = temp;}
    //the predicate function
    bool operator()(vgl_point_2d<unsigned> const& pa, vgl_point_2d<unsigned> const& pb) const
    {
        return vgl_distance<unsigned>(pa,focus_) < vgl_distance<unsigned>(pb,focus_);
    }
private:
    vgl_point_2d<unsigned> focus_;
};


class ncn_sampler
{
public:
    static bool sample_pivot_pixels(vnl_matrix<float> const& entropy_matrix, unsigned const& num_pivot_pixels,
                                vcl_set<vgl_point_2d<unsigned>,vgl_point_2d_less_than >& pivot_pixel_candidates, unsigned const& nparticles = 5000);

    static bool vgl_point_2d_set_to_dat(vcl_ofstream& os, vcl_set<vgl_point_2d<unsigned>,vgl_point_2d_less_than> const& point_set);

private:
    static unsigned find_bin(vbl_array_1d<double> const& cdf, double const& target);

};



#endif //ncn_sample_h_
