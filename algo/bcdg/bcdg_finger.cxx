// This is bcdg_finger.cxx
//:
// \file

#include <vcl_utility.h>
#include <vcl_cmath.h>
#include <dbecl/dbecl_episeg.h>
#include <dbecl/dbecl_episeg_point.h>
#include <vsol/vsol_digital_curve_2d.h>
#include "bcdg_finger.h"
#include "bcdg_local_option.h"

//: Constructor
bcdg_finger::bcdg_finger(dbecl_episeg_sptr seg, double idx, bcdg_direction d, int f)  : _curve(seg),
  _index(idx),
  _direction(d),
  _frame(f)
{}



//: Get the NEW finger for the given angle, localoption.
bcdg_finger_sptr bcdg_finger::next_finger(double angle, 
    const bcdg_local_option_sptr& loption)  const {
  return bcdg_finger_sptr(
      new bcdg_finger(loption->curve(), 
                     loption->curve()->index(angle), 
                     loption->direction(),
                      _frame));
}


//: Get the nearby local options
vcl_vector<bcdg_local_option_sptr> bcdg_finger::local_options(bcdg_algo_params p)  const {
  vcl_vector<bcdg_local_option_sptr> to_return;

  // Get nearby neighbors
  if(_curve != NULL && _curve->curve() != NULL) {
    vcl_vector< dbecl_episeg_point > nearby_points = 
      p->neighbors(_curve->curve()->interp(_index), _frame); 
  
    vcl_cerr << "Supposedly, there are " << nearby_points.size() << " neighbors!\n";
    
    for(int i = 0; i < nearby_points.size(); i++) {
      dbecl_episeg_point thisVal = nearby_points[i];
      to_return.push_back(bcdg_local_option_sptr(
            new bcdg_local_option(thisVal.episeg(), (double) thisVal.index(), DOWN)));
      to_return.push_back(bcdg_local_option_sptr(
            new bcdg_local_option(thisVal.episeg(), (double) thisVal.index(), UP)));
    }
  }

  if(_curve != NULL) {
    int nextIndex = (_direction == UP) ?  (int) vcl_floor(_index + 1) : 
                                          (int) vcl_ceil (_index - 1) ;
    to_return.push_back(bcdg_local_option_sptr(
        new bcdg_local_option(_curve, nextIndex, _direction)));
  }  

  return to_return;
}


