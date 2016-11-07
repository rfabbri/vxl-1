// This is algo/bcdg/bcdg_hypothel.h
#ifndef bcdg_hypothel_h_
#define bcdg_hypothel_h_

//:
// \file
// \brief  The smallest unit of a hypothesis
// \author scates and dapachec
// \date   7/26/2004

#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_vector.h>
#include <dbecl/dbecl_episeg_sptr.h>

#include "bcdg_hypothel_sptr.h"

//: The smallest unit of a hypothesis
class bcdg_hypothel : public vbl_ref_count {
public:
  //: Constructor description
  bcdg_hypothel(const vgl_point_3d<double>&, 
                const vcl_vector< dbecl_episeg_sptr > &,
                double d);
  
  //: Update the _next pointer
  void set_next(const bcdg_hypothel_sptr&);

  //: Follow the _next pointer
  bcdg_hypothel_sptr next_hypothel() const;

  //: Get the 3d point
  vgl_point_3d<double> point() const { return _point_3d; }

  //: Get the cost
  double cost() const { return _cost; }

protected:
  //: the 3D point
  vgl_point_3d<double> _point_3d;
  //: the curve in each frame that this corresponds to
  vcl_vector< dbecl_episeg_sptr > _curves_2d;
  //: pointer to the next one
  bcdg_hypothel_sptr _next;
  //: cost
  double _cost;
};


#endif // bcdg_hypothel_h_
