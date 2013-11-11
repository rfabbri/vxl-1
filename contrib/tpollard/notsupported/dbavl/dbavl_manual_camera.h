#ifndef _dbavl_manual_camera_h_
#define _dbavl_manual_camera_h_

//:
// \file
// \brief Manually compute cameras from known 3d world correspondences.
// \author Thomas Pollard
// \date 08/16/06
// 
//   This is....

#include <vcl_vector.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vpgl/vpgl_proj_camera.h>


class dbavl_manual_camera{

public:

  // Create with default parameters.
  dbavl_manual_camera();


  // Primary Operations:------------------------

 bool compute(
   const vcl_vector< vgl_point_3d<double> >& world_points,
   const vcl_vector< vgl_point_2d<double> >& image_points,
   const vcl_vector< vgl_line_segment_2d<double> >& up_lines,
   vpgl_proj_camera<double>& camera );

};


#endif // _dbavl_manual_camera_h_
