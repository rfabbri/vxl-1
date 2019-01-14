#ifndef _dbccl_camera_estimator_h_
#define _dbccl_camera_estimator_h_

//:
// \file
// \brief Compute cameras for video from tracks.
// \author Thomas Pollard
// \date 08/16/06
// 
//   This is....

#include <vector>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_point_2d.h>
#include <vpgl/vpgl_perspective_camera.h>


class dbccl_camera_estimator{

public:

  // Create with default parameters.
  dbccl_camera_estimator();


  // Primary Operations:------------------------

  // Estimate the cameras and world points.
  bool estimate(
    const std::vector< std::vector< vgl_point_2d<double> > >& tracks,
    const std::vector< std::vector<bool> >& track_masks,
    const vpgl_calibration_matrix<double>& K,
    std::vector< vpgl_perspective_camera<double> >& cameras );


  // Helper Functions:--------------------------

  // Find the scale of the translation of camera so that the points best line up.
  double find_best_camera(
    vpgl_perspective_camera<double>& camera,
    const std::vector< vgl_point_3d<double> >& world_points,
    const std::vector< vgl_point_2d<double> >& image_points );

};


#endif // _dbccl_camera_estimator_h_
