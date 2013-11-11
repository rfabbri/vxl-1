// This is algo/bcdg/bcdg_finger.h
#ifndef algo_bcdg_bcdg_finger_h_
#define algo_bcdg_bcdg_finger_h_

//:
// \file
// \brief   Represents a position along a curve while tracing multiple curves.
// \author  scates and dapachec
// \date    7/14/2004

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vcl_set.h>
#include <dbecl/dbecl_episeg_sptr.h>
#include "bcdg_local_option_sptr.h"
#include "bcdg_globals.h"
#include "bcdg_algo0.h"

//: Represents a position along a curve while tracing multiple curves.
class bcdg_finger : public vbl_ref_count {
public:
  //: Constructor
  bcdg_finger(dbecl_episeg_sptr, double, bcdg_direction, int framenum);
  
  //: Get the finger for the given angle and local option
  // (returns a new one)
  bcdg_finger_sptr next_finger(double, const bcdg_local_option_sptr&) const;

  //: Get the local options
  vcl_vector<bcdg_local_option_sptr> local_options(bcdg_algo_params) const;

  
private:
  //: Store the actual curve
  dbecl_episeg_sptr _curve;
  //: Where on the curve are we?
  double _index;
  //: In which direction are we going?
  bcdg_direction _direction;
  //: What frame are we in?
  int _frame;
};

typedef vbl_smart_ptr< bcdg_finger > bcdg_finger_sptr;

#endif // algo_bcdg_bcdg_finger_h_
