#ifndef _dbccl_fast_mi_h_
#define _dbccl_fast_mi_h_

//:
// \file
// \brief Code for doing fast mutual information computation.
// \author Thomas Pollard
// \date 08/16/06
// 
//   This is....

#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vil/vil_image_view.h>


class dbccl_fast_mi_params{

public:

  dbccl_fast_mi_params() :
    mi_mask_rad(5),
    histogram_size(10),
    mi_region_rad(50){}

  // Neighborhood of a pixel to use in MI computation.
  int mi_mask_rad;
  inline int mi_mask_diameter(){ return 1+2*mi_mask_rad; }
  inline int mi_mask_volume(){ return mi_mask_diameter()*mi_mask_diameter(); }

  // Number of buckets in histograms in MI computaiton.
  int histogram_size;

  // The radius of the region at which MI is computed at each pixel.
  int mi_region_rad;
  inline int mi_region_diameter(){ return 1+2*mi_region_rad; }
  inline int mi_region_volume(){ return mi_region_diameter()*mi_region_diameter(); }

};


class dbccl_fast_mi{

public:

  // Create with default parameters.
  dbccl_fast_mi( dbccl_fast_mi_params* params );


  // Primary Operations:------------------------

  float compute_mi(
    const vil_image_view<vxl_byte>& image,
    const vgl_point_2d<int>& image_window_ul_corner,
    const vnl_matrix<int>& mi_mask );

  void compute_mi_region(
    const vil_image_view<vxl_byte>& image,
    const vgl_point_2d<int>& starting_point,
    const vnl_matrix<int>& mi_mask,
    vnl_matrix<float>& mi_region );

protected:

  dbccl_fast_mi_params* params_;

  // Build a log table for speed boost.
  float log_search_volume_;
  vnl_vector<float> log_table_;

};


#endif // _dbccl_fast_mi_h_
