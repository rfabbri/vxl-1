// This is contrib/bvam/pro/bvam_ekf_camera_optimize_process.h
#ifndef bvam_ekf_camera_optimize_process_h_
#define bvam_ekf_camera_optimize_process_h_

//:
// \file
// \brief // A process that optimizes camera parameters based on image, voxel_world, and GPS/INS measurements
//           
// \author Daniel Crispell
// \date 02/26/08
// \verbatim
//
// \Modifications 

#include <vcl_string.h>
#include <bprb/bprb_process.h>

#include <vil/vil_image_view.h>
#include <vimt/vimt_transform_2d.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bvam/bvam_voxel_world.h>

class bvam_ekf_camera_optimize_process : public bprb_process
{
 public:
  
   bvam_ekf_camera_optimize_process();

  //: Copy Constructor (no local data)
  bvam_ekf_camera_optimize_process(const bvam_ekf_camera_optimize_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvam_ekf_camera_optimize_process(){};

  //: Clone the process
  virtual bvam_ekf_camera_optimize_process* clone() const {return new bvam_ekf_camera_optimize_process(*this);}

  vcl_string name(){return "BvamEkfCameraOptimize";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

   //: calculates the Jacobian of the homography wrt the rotation parameters as a function of K
   vnl_matrix<double> homography_jacobian_r(vnl_matrix_fixed<double,3,3> K);

   //: calculates the Jacobian of the homography wrt the translation parameters using finite differences
   vnl_matrix<double> homography_jacobian_t(bvam_voxel_world_sptr world, vpgl_perspective_camera<double>* cam, vil_image_view_base_sptr &expected_view);

   //: calculate a homography between two images
   vimt_transform_2d calculate_homography(vil_image_view_base_sptr &base_img, vil_image_view_base_sptr &img, vil_image_view_base_sptr &mask);


};



#endif // bvam_ekf_camera_optimize_process_h_

