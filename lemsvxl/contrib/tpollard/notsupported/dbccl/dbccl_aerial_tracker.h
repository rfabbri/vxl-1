#ifndef _dbccl_aerial_tracker_h_
#define _dbccl_aerial_tracker_h_

//:
// \file
// \brief A point tracker for aerial video.
// \author Thomas Pollard
// \date 08/16/06
// 
//   This is....

#include "dbccl_fast_mi.h"

#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vil/vil_image_view.h>

class dbccl_aerial_tracker_params{

public:

  dbccl_aerial_tracker_params() :
    max_rotation(.2),
    num_fm_trans_searches(100),
    num_fm_rot_searches(11){nbhd_rad=1;}

  // A slightly larger neighborhood from the starting image of a track, to construct
  // smaller nbhds with using warping.
  int nbhd_rad;
  inline int nbhd_diameter(){ return 1+2*nbhd_rad; }

  // Max rotation in the image plane between adjacent frames.
  float max_rotation;

  // Number of locations to search the space of fundamental matrices when tracking.
  int num_fm_trans_searches;
  int num_fm_rot_searches; // should be odd

};


class dbccl_aerial_tracker{

public:

  // Create with default parameters.
 dbccl_aerial_tracker();


  // Primary Operations:------------------------

  void track_frame(
    const vil_image_view<vxl_byte>& new_frame,
    const vcl_vector< vgl_point_2d<int> >& new_tracks );

  vcl_vector< vgl_point_2d<int> >& get_tracks(){ return tracks_; }


  // Helper Functions:--------------------------

  bool transform_window(
    const vnl_matrix<int>& wide_window,
    const vnl_matrix<float>& transform,
    vnl_matrix<int>& warped_window );

  dbccl_fast_mi_params mi_params;
  dbccl_aerial_tracker_params params;

protected:

  vcl_vector< vgl_point_2d<int> > tracks_;
  vcl_vector<bool> track_masks_;
  vcl_vector< vnl_matrix<float> > nbhd_transform_;
  vcl_vector< vnl_matrix<int> > nbhd_window_;
  vcl_vector< vnl_matrix<int> > mi_mask_;

};


#endif // _dbccl_aerial_tracker_h_
