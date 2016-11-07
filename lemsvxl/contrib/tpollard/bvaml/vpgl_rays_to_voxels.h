#ifndef _vpgl_rays_to_voxels_h_
#define _vpgl_rays_to_voxels_h_

//:
// \file
// \brief Given a voxel volume and a ray from a camera, this class
// computes which voxels are intersected by the ray.
// \author Thomas Pollard
// \date 012/21/06
// 
//   After constructing this class with a camera and voxel volume parameters
// one can use get_ray_voxels to return an list of voxel indices for all voxels 
// lying on a ray passing through an image pixel.  The list is sorted by 
// closeness to the camera center.  If each voxel needs to be associated with a 
// unique pixel, the option is available, though this no longer guarantees
// continuous voxel rays.


#include <vcl_vector.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vpgl/vpgl_perspective_camera.h>


class vpgl_rays_to_voxels {

public:

  //: Constructor takes the camera and the voxel volume dimensions.
  // The volume is assumed to be rectangular and aligned with the world
  // x,y,z axis with one corner at volume_corner.
  vpgl_rays_to_voxels(
    const vpgl_perspective_camera<double>& camera,
    const vgl_point_3d<double>& volume_corner,
    double voxel_length,
    const vgl_vector_3d<int>& num_voxels );

  //: Get all voxels along the ray from the camera coming from the
  // specified pixel.  When unique_assignment == true each voxel will
  // only be listed in one pixel-ray.
  void get_ray_voxels(
    const vgl_point_2d<int>& pixel_index,
    vcl_vector< vgl_point_3d<int> >& ray_voxels,
    bool unique_assignment );


protected:

  void get_ray_voxels(
    const vgl_point_2d<int>& pixel_index,
    vcl_vector< vgl_point_3d<int> >& ray_voxels );

  // Stored variables:
  vpgl_perspective_camera<double> camera_;
  vgl_point_3d<double> camera_center_;
  vgl_point_3d<double> volume_upper_corner_, volume_lower_corner_;
  double voxel_length_;
  vgl_point_3d<int> num_voxels_;
  int num_voxels_to_reserve_;

  // Rough bounds on the voxel volume projected into the image.
  int min_i_, max_i_, min_j_, max_j_;

};


#endif // _vpgl_rays_to_voxels_h_
