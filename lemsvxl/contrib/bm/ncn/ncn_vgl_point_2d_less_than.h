// this is /contrib/bm/ncn/ncn_vgl_point_2d_less_than.h
#ifndef ncn_vgl_point_2d_less_than_h_
#define ncn_vgl_point_2d_less_than_h_

#include<vgl/vgl_distance.h>
#include<vgl/vgl_point_2d.h>

class ncn_vgl_point_2d_less_than
{
public:
    ncn_vgl_point_2d_less_than(vgl_point_2d<unsigned> const& p): focus_(p){}
    ncn_vgl_point_2d_less_than(){vgl_point_2d<unsigned> temp(0,0); focus_ = temp;}
    //the predicate function
    bool operator()(vgl_point_2d<unsigned> const& pa, vgl_point_2d<unsigned> const& pb) const
    {
        return vgl_distance<unsigned>(pa,focus_) < vgl_distance<unsigned>(pb,focus_);
    }
private:
    vgl_point_2d<unsigned> focus_;
};

#endif //ncn_vgl_point_2d_less_than_h_
