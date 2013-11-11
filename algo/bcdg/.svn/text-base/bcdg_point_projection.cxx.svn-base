// This is algo/bcdg/bcdg_point_projection.cxx
//
//: \file

#include <vcl_cstdlib.h>
#include "bcdg_point_projection.h"

//: Constructor
bcdg_point_projection::bcdg_point_projection(
      const vgl_point_2d<double>& p1, 
      int t1,
      const vgl_point_2d<double>& p2,
      int t2,
      bcdg_algo0::params* parm
    )
  : _time(t1), _point(p1), 
    _depth(parm->epipole()->depth(p1,p2,vcl_abs(t2 - t1),parm->speed())),
    _params(parm)
{}

//: Get the depth of this point
double bcdg_point_projection::depth() const {
  return _depth;
}

//: Get the location of the point at the given time
vgl_point_2d<double> bcdg_point_projection::point(int i) const {
  return _params->epipole()->project(_point,_time,_depth,i,_params->speed());
}
