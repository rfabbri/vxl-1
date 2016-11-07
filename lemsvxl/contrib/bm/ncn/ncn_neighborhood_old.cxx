#include"ncn_neighborhood.h"
//this is /contrib/bm/ncn/ncn_neighborhood.cxx

bool ncn_neighborhood::get_region(unsigned const& x_tl, unsigned const& y_tl, unsigned const& x_lr, unsigned const& y_lr, vcl_vector<vgl_point_2d<unsigned> >& roi)
{
    for(unsigned y = y_tl; y <= y_lr; ++y)
        for(unsigned x=x_tl; x<= x_lr; ++x)
        {
            vgl_point_2d<unsigned> point(x,y);
            roi.push_back(point);
        }

     return true;
}

