
#include "bvam_mog_grey_processor.h"
#include "bvam_voxel_slab.h"
#include "bvam_voxel_slab_iterator.h"

//: Return probability density of observing pixel values
bvam_voxel_slab<float> 
bvam_mog_grey_processor::prob_density(bvam_voxel_slab<mix_gauss_type> const& appear,
                  bvam_voxel_slab<float> const& obs)
{       
  //the ouput
  bvam_voxel_slab<float> probabilities(appear.nx(), appear.ny(), appear.nz());

  //the slab iterators
  bvam_voxel_slab<mix_gauss_type>::const_iterator appear_it; 
  bvam_voxel_slab<float>::const_iterator obs_it = obs.begin();
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
bvam_mog_grey_processor::prob_range(bvam_voxel_slab<mix_gauss_type> const& appear,
                                    bvam_voxel_slab<float> const& obs_min,
                                    bvam_voxel_slab<float> const& obs_max)
{
  //the ouput
  bvam_voxel_slab<float> probabilities(appear.nx(), appear.ny(), appear.nz());

  //the slab iterators
  bvam_voxel_slab<mix_gauss_type>::const_iterator appear_it; 
  bvam_voxel_slab<float>::const_iterator min_it = obs_min.begin();
  bvam_voxel_slab<float>::const_iterator max_it = obs_max.begin();
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
bool bvam_mog_grey_processor::update( bvam_voxel_slab<mix_gauss_type> &appear,
            bvam_voxel_slab<float> const& obs,
            bvam_voxel_slab<float> const& weight)

{

  // the model
  float init_variance = 0.008f;
  float min_variance = 4.0e-4f;
  float g_thresh = 2.5; // number of std devs from mean sample must be
  bsta_gauss_f1 this_gauss(0.0f, init_variance);

 // the updater
  bsta_mg_grimson_weighted_updater<mix_gauss> updater(this_gauss,this->n_gaussian_modes_,g_thresh,min_variance);

  //check dimensions match
  assert(appear.nx() == obs.nx());
  assert(appear.ny() == obs.ny());
  assert(updater.data_dimension == appear.nz());

  //the iterators
  bvam_voxel_slab<mix_gauss_type>::iterator appear_it; 
  bvam_voxel_slab<float>::const_iterator obs_it = obs.begin();
  bvam_voxel_slab<float>::const_iterator weight_it = weight.begin();


  for(appear_it = appear.begin(); appear_it!= appear.end(); ++appear_it, ++obs_it, ++weight_it)
  {
    if (*weight_it > 0)
      updater(*appear_it, *obs_it, *weight_it);
  }
  return true;
}


//: Expected value 
bvam_voxel_slab<float> bvam_mog_grey_processor::expected_color(bvam_voxel_slab<mix_gauss_type > const& appear)
{       
  //the output
   bvam_voxel_slab<float> expected_color(appear.nx(),appear.ny(),appear.nz());

   //the iterator
   bvam_voxel_slab<mix_gauss_type>::const_iterator appear_it; 
   bvam_voxel_slab<float>::iterator ec_it = expected_color.begin();

  

  for(appear_it = appear.begin(); appear_it!= appear.end();++appear_it, ++ec_it)
  {
   float total_weight= 0;
   float c = 0;

    //should be components used
    for(unsigned i = 0 ; i< (*appear_it).num_components(); i++)
    {
      total_weight += (*appear_it).weight(i);
      c+= (*appear_it).distribution(i).mean() * (*appear_it).weight(i); 
      
    }
    if (total_weight > 0.0f)
      (*ec_it)= c/total_weight;
  }

  return expected_color;
}
