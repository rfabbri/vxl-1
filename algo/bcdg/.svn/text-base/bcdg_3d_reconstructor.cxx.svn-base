// This is bcdg_3d_reconstructor.cxx
//:
// \file

// MUCH of this code is based on vxl/contrib/brl/bseg/bmrf/bmrf_curve_builder

#include "bcdg_3d_reconstructor.h"
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_double_4.h>
#include <vnl/algo/vnl_svd.h>

//: Construct a reconstructor
bcdg_3d_reconstructor::bcdg_3d_reconstructor(const bcdg_global_option* g,
                                              const bcdg_algo_params p) :
params_(p)
{
  init_intrinsic();
  init_cameras();
  reconstruct(g);
}



//: Get the 3D point
vgl_point_3d<double> bcdg_3d_reconstructor::point_3d()  const {
  return point_;
}

//: Initialize intrinsic camera parameters
void bcdg_3d_reconstructor::init_intrinsic() {
  // For our camera images are 1024 x 768
  // The focal length is 12.5mm / (6.25 um/pixel) = 2000 pixels
  K_[0][0] = 2000;  K_[0][1] = 0;     K_[0][2] = 512;
  K_[1][0] = 0;     K_[1][1] = 2000;  K_[1][2] = 384;
  K_[2][0] = 0;     K_[2][1] = 0;     K_[2][2] = 1;
}

//: Initialize camera matricies
void bcdg_3d_reconstructor::init_cameras() {
   C_.clear();
   const vgl_point_2d<double>& ep = params_->epipole()->location();
 
   // compute the cameras
   vnl_double_3x4 E;
   E[0][0] = 1;  E[0][1] = 0;  E[0][2] = 0;  E[0][3] = ep.x();
   E[1][0] = 0;  E[1][1] = 1;  E[1][2] = 0;  E[1][3] = ep.y();
   E[2][0] = 0;  E[2][1] = 0;  E[2][2] = 1;  E[2][3] = 1;
 
   vnl_double_3x4 Ef = K_*E;
   for(int i = 0; i < params_->num_frames(); i++) {
     int dt = 1;
     Ef[0][3] = dt*ep.x();
     Ef[1][3] = dt*ep.y();
     Ef[2][3] = dt;
     C_[i] = Ef;
   }
}

//: Do the actual reconstruction of a 3D point from the set of 2D points.
void bcdg_3d_reconstructor::reconstruct(const bcdg_global_option* glob) {
  unsigned int nviews = params_->num_frames();
  vnl_matrix<double> A(2*nviews, 4, 0.0);

  unsigned int v = 0;
  for ( vcl_map<int,vnl_double_3x4>::const_iterator C_itr = C_.begin();
        C_itr != C_.end();  ++C_itr ) {
    const int f = C_itr->first;
    const vnl_double_3x4 cam = C_itr->second;
    vgl_point_2d<double> pos = glob->point_in_frame(f);
    
    for (unsigned int i=0; i<4; i++) {
      A[2*v  ][i] = pos.x()*cam[2][i] - cam[0][i];
      A[2*v+1][i] = pos.y()*cam[2][i] - cam[1][i];
    }
    ++v;
  }
  vnl_svd<double> svd_solver(A);
  vnl_double_4 p = svd_solver.nullvector();

  point_ = vgl_point_3d<double>(p[0]/p[3], p[1]/p[3], p[2]/p[3]);
}
