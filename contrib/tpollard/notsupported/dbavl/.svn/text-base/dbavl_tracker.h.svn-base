#ifndef _dbavl_tracker_h_
#define _dbavl_tracker_h_

//:
// \file
// \brief A point tracker for aerial video
// \author Thomas Pollard
// \date 08/16/06
// 
//   This is....

#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vil/vil_image_view.h>

class dbavl_tracker_params{

public:

  dbavl_tracker_params() :
    mi_mask_rad(5),
    nbhd_rad(10),
    histogram_size(10),
    search_rad(50),
    max_rotation(.2),
    num_fm_trans_searches(100),
    num_fm_rot_searches(11){}

  // Neighborhood of a pixel to use in MI computation.
  int mi_mask_rad;
  inline int mi_mask_diameter(){ return 1+2*mi_mask_rad; }

  // A slightly larger neighborhood from the starting image of a track, to construct
  // smaller nbhds with using warping.
  int nbhd_rad;
  inline int nbhd_diameter(){ return 1+2*nbhd_rad; }

  // Number of buckets in histograms in MI computaiton.
  int histogram_size;

  // The maximal movement of a pixel between adjacent frames.
  int search_rad;
  inline int search_diameter(){ return 1+2*search_rad; }
  inline int search_volume(){ return search_diameter()*search_diameter(); }

  // Max rotation in the image plane between adjacent frames.
  float max_rotation;

  // Number of locations to search the space of fundamental matrices when tracking.
  int num_fm_trans_searches;
  int num_fm_rot_searches; // should be odd

};


class dbavl_tracker{

public:

  // Create with default parameters.
  dbavl_tracker();


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

  float compute_mi(
    const vil_image_view<vxl_byte>& image,
    const vgl_point_2d<int>& image_window_ul_corner,
    const vnl_matrix<int>& mi_mask );

  void compute_region_mi(
    const vil_image_view<vxl_byte>& image,
    const vgl_point_2d<int>& starting_point,
    const vnl_matrix<int>& mi_mask,
    vnl_matrix<float>& region_mi );

  dbavl_tracker_params params;

protected:

  vcl_vector< vgl_point_2d<int> > tracks_;
  vcl_vector<bool> track_masks_;
  vcl_vector< vnl_matrix<float> > nbhd_transform_;
  vcl_vector< vnl_matrix<int> > nbhd_window_;
  vcl_vector< vnl_matrix<int> > mi_mask_;

  // Build a log table for speed boost.
  float log_search_volume;
  vnl_vector<float> log_table;

};


#endif // _dbavl_tracker_h_
