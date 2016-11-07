// This is contrib/isabel/bseg3d/algo/bseg3d_window_detector.h
#ifndef bseg3d_window_detector_h_
#define bseg3d_window_detector_h_
//:
// \file
// \brief class to probabilistically detect regions in 3d world
// \author Isabel Restrepo
// \date May 4, 2009
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
#include <vpdl/vpdl_gaussian_indep.h>


class bseg3d_window_detector
{

private:
  //define some known mixtures
  typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;


public:
  template <unsigned dim>
  float difference_probability(vcl_vector<gauss_type> region1,vcl_vector<gauss_type> region2);

};



template <unsigned dim>
float bseg3d_window_detector::difference_probability(vcl_vector<gauss_type> region1,vcl_vector<gauss_type> region2)
{
  vnl_vector_fixed<float, dim> mu1;
  vnl_matrix<float> cov1(region1.size(),region1.size(),float(0));
  vnl_vector_fixed<float,dim> mu2;
  vnl_matrix<float> cov2(region2.size(),region2.size(),float(0));

  //fill in the values

  for(unsigned i = 0; i < region1.size(); ++i)
  {
    mu1[i] = region1[i].mean();
    cov1[i][i]= region1[i].var();
  }
  for(unsigned i = 0; i < region2.size(); ++i)
  {
    mu2[i] = region2[i].mean();
    cov2[i][i]= region2[i].var();
  }

  bsta_gaussian_indep<float,dim> gauss1(mu1, cov1);
  bsta_gaussian_indep<float,dim> gauss2(mu2, cov2);

  vnl_vector_fixed<float,dim> temp(float(1.0));
  vnl_vector_fixed<float,dim> temp2(float(-1.0));
  vpdl_gaussian_indep<float,dim> gauss_out(mu1- mu2, cov1-cov2);
  gauss_out.cumulative_prob(const cov1-cov2*temp)
  return 1 -(gauss_out.cumulative_prob(const cov1-cov2*temp) -
  gauss_out.cumulative_prob(const cov1-cov2*temp2));
}


#endif

