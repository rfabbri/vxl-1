// This is contrib/isabel/bseg3d/algo/bseg3d_convolve_mask.h
#ifndef bseg3d_convolve_mask_h_
#define bseg3d_convolve_mask_h_
//:
// \file
// \brief class to convolve a bvxm-grid with a 3d massk
// \author Isabel Restrepo
// \date April 6, 2009
//
//
// \verbatim
//  Modifications
// \endverbatim
#include <bvxm/grid/bvxm_voxel_grid_base.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_gauss_if3.h>
class bseg3d_convolve_mask
{
private:
  //define some known mixtures
  //typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
  //typedef bsta_gaussian_indep<float,3> bsta_gauss_if3;

public:
  
 /* bool convolve(bvxm_grid_base_sptr grid_in,
                bvxm_grid_base_sptr mask_in,
                bvxm_grid_base_sptr grid_out);*/
  //Retruns the convolution of two multivariete normal distributions
  //this function should be chagend to use the new run time capabilities of vpdl
  bsta_gauss_if3 gaussian_regions( bsta_gauss_if3 const &R1, bsta_gauss_if3 const &R2);
 
};

#endif

