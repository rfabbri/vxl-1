
#include "bvam_mog_rgb_processor.h"
#include "bvam_voxel_slab.h"
#include "bvam_voxel_slab.txx"
#include "bvam_voxel_slab_iterator.h"

//: Return probability density of observing pixel values
bvam_voxel_slab<float> 
bvam_mog_rgb_processor::prob_density(bvam_voxel_slab<apm_datatype> const& appear,
                  bvam_voxel_slab<obs_datatype> const& obs)
{       
  //the ouput
  bvam_voxel_slab<float> probabilities(appear.nx(), appear.ny(), appear.nz());

  //the slab iterators
  bvam_voxel_slab<apm_datatype>::const_iterator appear_it; 
  bvam_voxel_slab<obs_datatype>::const_iterator obs_it = obs.begin();
  bvam_voxel_slab<float>::iterator prob_it = probabilities.begin();


  for(appear_it = appear.begin(); appear_it!= appear.end(); ++appear_it, ++obs_it, ++prob_it)
  {
    if ((*appear_it).num_components() ==0)
      (*prob_it)= 1.00f;
    else
    (*prob_it) = (*appear_it).prob_density(*obs_it);
   }

  return probabilities;

}

//: Return probabilities that pixels are in range [min,max]
bvam_voxel_slab<float> 
bvam_mog_rgb_processor::prob_range(bvam_voxel_slab<apm_datatype> const& appear,
                                    bvam_voxel_slab<obs_datatype> const& obs_min,
                                    bvam_voxel_slab<obs_datatype> const& obs_max)
{
  //the ouput
  bvam_voxel_slab<float> probabilities(appear.nx(), appear.ny(), appear.nz());

  //the slab iterators
  bvam_voxel_slab<apm_datatype>::const_iterator appear_it; 
  bvam_voxel_slab<obs_datatype>::const_iterator min_it = obs_min.begin();
  bvam_voxel_slab<obs_datatype>::const_iterator max_it = obs_max.begin();
  bvam_voxel_slab<float>::iterator prob_it = probabilities.begin();

  for(appear_it = appear.begin(); appear_it!= appear.end(); ++appear_it, ++min_it, ++max_it, ++prob_it)
  {
    if ((*appear_it).num_components() ==0)
      (*prob_it)= 1.00f;
    else
    (*prob_it) = (*appear_it).probability(*min_it,*max_it);
   }

  return probabilities;

}



//: Update with a new sample image
bool bvam_mog_rgb_processor::update( bvam_voxel_slab<apm_datatype> &appear,
            bvam_voxel_slab<obs_datatype> const& obs,
            bvam_voxel_slab<float> const& weight)

{

  // the model
  bsta_gauss_if3 init_gauss(obs_datatype(0.0f),obs_datatype(0.01f));

 // the updater
  bsta_mg_grimson_weighted_updater<mix_gauss> updater(init_gauss);

  //check dimensions match
  assert(appear.nx() == obs.nx());
  assert(appear.ny() == obs.ny());

  //the assert does n't seem to be right : Please CHECK DAN
  // assert(updater.data_dimension == appear.nz());

  //the iterators
  bvam_voxel_slab<apm_datatype>::iterator appear_it; 
  bvam_voxel_slab<obs_datatype>::const_iterator obs_it = obs.begin();
  bvam_voxel_slab<float>::const_iterator weight_it = weight.begin();


  for(appear_it = appear.begin(); appear_it!= appear.end(); ++appear_it, ++obs_it, ++weight_it)
  {
    updater(*appear_it, *obs_it, *weight_it);
  }
  return true;
}


//: Expected value 
bvam_voxel_slab<bvam_mog_rgb_processor::obs_datatype> bvam_mog_rgb_processor::expected_color(bvam_voxel_slab<mix_gauss_type > const& appear)
{       
  //the output
   bvam_voxel_slab<obs_datatype> expected_color;

   //the iterator
   bvam_voxel_slab<apm_datatype>::const_iterator appear_it; 
   bvam_voxel_slab<obs_datatype>::iterator ec_it = expected_color.begin();


  for(appear_it = appear.begin(); appear_it!= appear.end();++appear_it, ++ec_it)
  {
   obs_datatype total_weight,c;
   total_weight.fill(0);
   c.fill(0);

    //should be components used
    for(unsigned i = 0 ; i< (*appear_it).num_components(); i++)
    {
      total_weight += (*appear_it).weight(i);
      c+= (*appear_it).distribution(i).mean() * (*appear_it).weight(i); 
      
    }
    (*ec_it)= (1/total_weight.magnitude())*c;
   }

  return expected_color;
}

