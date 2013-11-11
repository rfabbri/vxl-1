// This is / bvma_mog_grey_processor.h
#ifndef bvam_mog_grey_processor_h_
#define bvam_mog_grey_processor_h_

//:
// \file
// \brief // A class for a grey-scale-mixture-of-gaussian processor
//           
// \author Isabel Restrepo
// \date 01/14/ 08
// \verbatim
//
// \Modifications 
// Changed prob() to prob_density(), added prob_range      DEC 02/11/2008

//Added the get_light_bin() virtual function which is useful in multiple mixture of gaussian case 
//to return the appropriate bin number from the lighting direction


#include "bvam_voxel_slab.h"

#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_gaussian_indep.h>


// The mix_gauss_type contains the same data as mix_gauss plus an
// extra attribute that indicate the number of observations

class  bvam_mog_grey_processor
{
protected:
  static const unsigned n_gaussian_modes_ = 3;

  typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, n_gaussian_modes_> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;
public: 

  typedef mix_gauss_type apm_datatype;
  typedef float obs_datatype;
  typedef float obs_mathtype;

  bvam_mog_grey_processor(){};

  bvam_voxel_slab<float>  prob_density(bvam_voxel_slab<mix_gauss_type> const& appear,
    bvam_voxel_slab<float> const& obs);

  bvam_voxel_slab<float> prob_range(bvam_voxel_slab<mix_gauss_type> const& appear,
    bvam_voxel_slab<float> const& obs_min,
    bvam_voxel_slab<float> const& obs_max);

  bool update( bvam_voxel_slab<mix_gauss_type> &appear,
    bvam_voxel_slab<float> const& obs,
    bvam_voxel_slab<float> const& weight);

  bvam_voxel_slab<float> expected_color( bvam_voxel_slab<mix_gauss_type> const& appear);

  //bin number is always 0 for the simple mixture of gaussian case
  virtual unsigned int get_light_bin(unsigned int num_light_bins, const vnl_vector<float>& light) {return 0;}




};

#endif // bvma_mog_grey_processor_h_

