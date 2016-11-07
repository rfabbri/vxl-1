// This is / bvma_mog_rgb_processor.h
#ifndef bvam_mog_rgb_processor_h_
#define bvam_mog_rgb_processor_h_

//:
// \file
// \brief // A class for a rgb-mixture-of-gaussian processor
//           
// \author Pradeep
// \date 02/22/ 08
// \verbatim
//


#include "bvam_voxel_slab.h"

#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_gaussian_indep.h>

typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
typedef bsta_num_obs<mix_gauss> mix_gauss_type;
//typedef gauss_type::vector_type T;


// The mix_gauss_type contains the same data as mix_gauss plus an
// extra attribute that indicate the number of observations

class  bvam_mog_rgb_processor
{
public: 

  typedef mix_gauss_type apm_datatype;
  typedef gauss_type::vector_type obs_datatype;
  typedef gauss_type::math_type obs_mathtype;

  bvam_mog_rgb_processor(){};

  bvam_voxel_slab<float>  prob_density(bvam_voxel_slab<apm_datatype> const& appear,
                                       bvam_voxel_slab<obs_datatype> const& obs);

  bvam_voxel_slab<float> prob_range(bvam_voxel_slab<apm_datatype> const& appear,
                                    bvam_voxel_slab<obs_datatype> const& obs_min,
                                    bvam_voxel_slab<obs_datatype> const& obs_max);

  bool update( bvam_voxel_slab<apm_datatype> &appear,
    bvam_voxel_slab<obs_datatype> const& obs,
    bvam_voxel_slab<float> const& weight);

   bvam_voxel_slab<obs_datatype> expected_color( bvam_voxel_slab<mix_gauss_type> const& appear);

  //bin number is always 0 for the simple mixture of gaussian case
  virtual unsigned int get_light_bin(unsigned int num_light_bins, const vnl_vector<float>& light) {return 0;}
   

};

#endif // bvma_mog_rgb_processor_h_


