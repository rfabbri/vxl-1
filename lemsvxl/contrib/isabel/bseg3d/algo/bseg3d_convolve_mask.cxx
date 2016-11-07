#include "bseg3d_convolve_mask.h"


//bool bseg3d_convolve_mask::convolve_separable(bvxm_grid_base_sptr grid_in,
//                                              vcl_vector<vcl_vector> operators,
//                                              bvxm_grid_base_sptr grid_out)
//{
//  //cast grids
//
//
//  //get x, y, z convolving vectors
//
//  //convolve x
//
//  //convolve y 
//
//  //convolve z
//
//}

  bsta_gauss_if3 bseg3d_convolve_mask::gaussian_regions( bsta_gauss_if3 const &R1, bsta_gauss_if3 const &R2)
  {

    return bsta_gauss_if3(R1.mean() - R2.mean(), R1.covar() - R1.covar());
  }
