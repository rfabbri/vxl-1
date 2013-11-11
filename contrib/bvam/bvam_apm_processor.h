// This is /bvma_apm.h
#ifndef bvma_apm_processor_h_
#define bvma_apm_processor_h_

//:
// \file
// \brief // A base class for an appereance-model processor
//           
// \author Isabel Restrepo
// \date 01/14/ 08
// \verbatim
//
// \Modifications 

#include "bvam_voxel_slab.h"
#include "bvam_voxel_slab.h"

class bvam_apm_processor
{
public: 

  virtual bvam_voxel_slab<float> prob(bvam_voxel_slab_base &appear,
                                      bvam_voxel_slab_base &obs)=0;

  
  virtual bool update( bvam_voxel_slab_base &appear,
            bvam_voxel_slab_base &obs,
            bvam_voxel_slab<float> &weight)=0;

  virtual  bvam_voxel_slab<float> expected_color( bvam_voxel_slab_base &appear)=0;

  //virtual bool apply()
};

#endif // bvma_apm_processor_h_

