//this is contrib/bm/dncn/dncn_less_than_h_
#ifndef dncn_less_than_h_
#define dncn_less_than_h_

#include<vgl/vgl_distance.h>
#include<vgl/vgl_point_2d.h>

class dncn_less_than
{
public:
    dncn_less_than( vgl_point_2d<unsigned> const& p ): focus_(p){}

    dncn_less_than(){ vgl_point_2d<unsigned> temp(0,0); focus_ = temp;}

    bool operator()(vgl_point_2d<unsigned> const& pa, vgl_point_2d<unsigned> const& pb) const
    {
        return vgl_distance<unsigned>(pa, focus_) < vgl_distance<unsigned>(pb,focus_);
    }
private:
    vgl_point_2d<unsigned> focus_;
};

#endif //dncn_less_than_h_