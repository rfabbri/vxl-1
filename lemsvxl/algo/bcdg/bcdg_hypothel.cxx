// This is bcdg_hypothel.cxx
//:
// \file

#include "bcdg_hypothel.h"
#include "bcdg_costs.h"

//: Constructor
bcdg_hypothel::bcdg_hypothel( const vgl_point_3d<double>& p, 
                              const vcl_vector< dbecl_episeg_sptr > & v,
                              double cost) :
  _point_3d(p),
  _curves_2d(v),
  _cost(cost)
{ 
}



//: Set the "next" pointer
void bcdg_hypothel::set_next(const bcdg_hypothel_sptr& s)  {
  _next = s;
}



//: Follow the "next" pointer
bcdg_hypothel_sptr bcdg_hypothel::next_hypothel()  const {
  return _next;
}

