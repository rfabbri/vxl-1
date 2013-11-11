// This is algo/bcdg/bcdg_point_projection.h
#ifndef bcdg_point_projection_h_
#define bcdg_point_projection_h_
//:
// \file
// \brief   Represents the projection of a 3D point into multiple frames.
// \author  scates and dapachec
// \date    7/9/04
//

#include <vcl_vector.h>
#include <dbecl/dbecl_epipole.h>
#include <dbecl/dbecl_epipole_sptr.h>
#include <vgl/vgl_point_2d.h>
#include "bcdg_algo0.h"

//: Represents the projection of a 3D point into multiple frames.

class bcdg_point_projection {
public:
  //: Constructor
  bcdg_point_projection(const vgl_point_2d<double>&, int, 
                        const vgl_point_2d<double>&, int, 
                        bcdg_algo_params p);
  bcdg_point_projection() {}
  //: the depth of this point
  double depth() const;
  //: the location of the point at a given time
  vgl_point_2d<double> point(int) const;
  //: Sort by depth
  bool operator<(const bcdg_point_projection& rhs) const {
    return depth() < rhs.depth();
  }
private:
  //: The point
  vgl_point_2d<double> _point;
  //: The deltaT
  int _time;
  //: The depth
  double _depth;
  //: Params
  bcdg_algo_params _params;
};

#endif
