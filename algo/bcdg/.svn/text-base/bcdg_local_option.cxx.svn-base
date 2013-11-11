// This is algo/bcdg/bcdg_local_option.cxx
//:
// \file

#include "bcdg_local_option.h"
#include <dbecl/dbecl_episeg.h>
#include <vsol/vsol_digital_curve_2d.h>

//: Constructor
bcdg_local_option::bcdg_local_option(dbecl_episeg_sptr c, double i, bcdg_direction d) :
_curve(c),
_index(i),
_direction(d)
{}



//: Get the interpolated point at the given theta
vgl_point_2d<double> bcdg_local_option::at_theta(double t)  const {
  return _curve->curve()->interp(_curve->index(t));
}



//: Get the theta of the given point
double bcdg_local_option::theta()  const {
  return _curve->angle(_index);
}

//: Get the curve
dbecl_episeg_sptr bcdg_local_option::curve() const {
  return _curve;
}

//: Get the direction of this option
bcdg_direction bcdg_local_option::direction() const {
  return _direction;
}
