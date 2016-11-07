//this is contrib/bm/ncn/ncn_neighborhood.h
#ifndef ncn_neighborhood_h_
#define ncn_neighborhood_h_
//:
// \file
// \brief A class to store a non compact neighborhood
// \Author Brandon A. Mayer
// \date June 6, 2010
//
// \verbatim
//  No modifications yet.
// \endverbatim
#include<bsta/bsta_histogram.h>
#include<bsta/bsta_joint_histogram.h>

#include<vcl_iomanip.h>
#include<vcl_iostream.h>

#include<vcl_map.h>
#include<vcl_set.h>
#include<vcl_utility.h>
#include<vcl_string.h>

#include<vgl/vgl_distance.h>
#include<vgl/vgl_point_2d.h>

#include<vil/vil_convert.h>
#include<vil/vil_image_view.h>
#include<vil/vil_load.h>
#include<vil/vil_save.h>

#include<vnl/vnl_math.h>
#include<vnl/vnl_random.h>
#include<vnl/io/vnl_io_matrix.h>

class vgl_point_2d_lessThan
{
public:
    vgl_point_2d_lessThan(vgl_point_2d<unsigned> const& p): focus_(p){}
    vgl_point_2d_lessThan(){vgl_point_2d<unsigned> temp(0,0); focus_ = temp;}
    //the predicate function
    bool operator()(vgl_point_2d<unsigned> const& pa, vgl_point_2d<unsigned> const& pb) const
    {
        return vgl_distance<unsigned>(pa,focus_) < vgl_distance<unsigned>(pb,focus_);
    }
private:
    vgl_point_2d<unsigned> focus_;
};

namespace ncn_typedefs{
    typedef vcl_vector<vgl_point_2d<unsigned> > target_pixel_type;
    typedef vcl_set<vgl_point_2d<unsigned>,vgl_point_2d_lessThan> pivot_pixel_candidate_type;
    typedef vcl_map<vcl_vector<vgl_point_2d<unsigned> >::const_iterator,vcl_set<vgl_point_2d<unsigned>,vgl_point_2d_lessThan>::const_iterator> neighborhood_type;
}

using namespace ncn_typedefs;

class ncn_neighborhood
{
public:
    ncn_neighborhood();

    ~ncn_neighborhood();

    bool get_region(unsigned const& x_tl, unsigned const& y_tl, unsigned const& x_lr, unsigned const& y_lr, vcl_vector<vgl_point_2d<unsigned> >&roi);


private:
    target_pixel_type target_pixels_;
    pivot_pixel_candidate_type pivot_pixel_candidates_;
    neighborhood_type neighborhood_;
    
};

#endif //ncn_neighborhood_h_