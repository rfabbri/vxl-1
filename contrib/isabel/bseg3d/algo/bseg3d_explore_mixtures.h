// This is contrib/isabel/bseg3d/algo/bseg3d_explore_mixtures.h
#ifndef bseg3d_explore_mixtures_h_
#define bseg3d_explore_mixtures_h_
//:
// \file
// \brief class to explore the gaussian mixtures in the appereance model of a bvxm world
// \author Isabel Restrepo
// \date March 10, 2009
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <bvxm/bvxm_util.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/grid/bvxm_voxel_grid_base.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_voxel_slab.h>
#include <bvxm/bvxm_world_params.h>


class bseg3d_explore_mixtures
{

private:
  //define some known mixtures
  typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;
  typedef float datatype_;
public:
  //: Thresholds an input grid. Outputs as thresholded griad and a mask such that
  //  grid_out = grid_in * mask
  bool get_region_above_threshold(bvxm_voxel_grid_base_sptr grid_in_base,
                                  bvxm_voxel_grid_base_sptr grid_out_base,
                                  bvxm_voxel_grid_base_sptr mask_base,
                                  float min_thresh);

  //: Computes the l2distances between the mistures of gaussians of a grid.
  //  If no mixture is given as a reference, the first mixture in the grid is used
  bool calculate_l2distances(bvxm_voxel_grid_base_sptr apm_grid_base,
                             bvxm_voxel_grid_base_sptr mask_grid_base,
                             bvxm_voxel_grid_base_sptr dist_grid_base,
                             bool reference_given,
                             mix_gauss_type reference  = mix_gauss_type());
  
  //: Merges the components of the gaussian mixtures at each voxel into a single gaussian
  //  thus the resulting grid contains unimodal gaussians
  bool merge_mixtures(bvxm_voxel_grid_base_sptr app_grid,
                    bvxm_voxel_grid_base_sptr gauss_app) ;



  //: Saves a floating point grid in binary format for Drishti Volume Rederer
  bool save_float_grid_raw(bvxm_voxel_grid_base_sptr grid_base,vcl_string filename);
 
  bool save_byte_grid_raw(bvxm_voxel_grid_base_sptr grid_base,vcl_string filename);
  //: Retieves the expected image of a grid  
 /* bool bseg3d_explore_mixtures::expected_image(vpgl_camera_double_sptr const& camera,
                                               vil_image_view_base_sptr &expected,
                                               bvxm_voxel_grid_base_sptr grid_base,
                                               bvxm_voxel_grid_base_sptr ocp_grid_base,
                                               bvxm_world_params_sptr params);*/

};

#endif


