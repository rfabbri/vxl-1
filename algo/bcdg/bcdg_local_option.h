// This is algo/bcdg/bcdg_local_option.h
#ifndef algo_bcdg_bcdg_local_option_h_
#define algo_bcdg_bcdg_local_option_h_

//:
// \file
// \brief   A possible next point on a particular curve (in one finger)
// \author  scates and dapachec
// \date    7/14/2004

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <dbecl/dbecl_episeg_sptr.h>
#include <vgl/vgl_point_2d.h>
#include "bcdg_globals.h"

//: A possible next point on a particular curve (in one finger)
class bcdg_local_option : public vbl_ref_count {
public:
  //: Constructor description
  bcdg_local_option(dbecl_episeg_sptr, double, bcdg_direction);
  //: Get the point
  vgl_point_2d<double> at_theta(double) const;
  //: Get the theta of the given point
  double theta() const;
  //: Get the curve
  dbecl_episeg_sptr curve() const;
  //: Get the direction
  bcdg_direction direction() const;

protected:
  //: The curve we're on
  dbecl_episeg_sptr _curve;
  //: The index of the point (into the underlying curve)
  double _index;
  //: The direction of this option
  bcdg_direction _direction;
};

typedef vbl_smart_ptr< bcdg_local_option > bcdg_local_option_sptr;

#endif // algo_bcdg_bcdg_local_option_h_
