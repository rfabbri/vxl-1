// This is algo/bcdg/bcdg_3d_reconstructor.h
#ifndef algo_bcdg_bcdg_3d_reconstructor_h_
#define algo_bcdg_bcdg_3d_reconstructor_h_

//:
// \file
// \brief  This class reconstructs a 3D point from a set of 2D points.
// \author dapachec
// \date   8/17/2004

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vcl_map.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x4.h>
#include <vgl/vgl_point_3d.h>

#include "bcdg_algo0.h"
#include "bcdg_global_option.h"

//: This class reconstructs a 3D point from a set of 2D points.
class bcdg_3d_reconstructor : public vbl_ref_count {
public:
  //: Construct a reconstructor from a global option
  bcdg_3d_reconstructor(const bcdg_global_option*, const bcdg_algo_params);

  //: Get the 3D point associated with the given 2D points.
  vgl_point_3d<double> point_3d() const;

protected:
  //: Initializers
  void init_intrinsic();
  //: Initializers
  void init_cameras();
  //: Reconstruct a point
  void reconstruct(const bcdg_global_option*);
  
  //: Store the 3D point
  vgl_point_3d<double> point_;

  //: Store parameters
  bcdg_algo_params params_;
  
  //: Camera intrinsic parameters
  vnl_double_3x3 K_;

  //: Map from frame numbers to cameras
  vcl_map<int,vnl_double_3x4> C_;
};

typedef vbl_smart_ptr< bcdg_3d_reconstructor > bcdg_reconstructor_sptr;

#endif // bcdg_reconstructor_h_
