#include "bseg3d_merge_mixtures.h"
//:
// \file
#include <vnl/vnl_math.h>
#include <vcl_cmath.h>
#include <vcl_vector.h>

void bseg3d_merge_mixtures::all_to_one(mix_gauss_type const& mixture, bsta_gauss_f1 &gaussian)
{
  //When merging components of gaussian mixtures, we need to perform the followign calculations
  //1. find the normalizing weight of the new component. In this case this value is 1 because
  //   we are merging all the componets 
  //2. find the mean 
  //3. find the variance

  float mean = 0.0f;
  float var = 0.0f;
  //Find mean and variance
  for(unsigned i = 0; i<mixture.num_components(); i++)
  {
    //note these equations ommit the weight of the final distribution
    // only becausee in this case it is 1
    mean = mean + mixture.weight(i)*mixture.distribution(i).mean();
    var = var +  mixture.weight(i)*mixture.distribution(i).var() + 
      mixture.weight(i)*vcl_pow(mixture.distribution(i).mean(),2);
    
  }
 
  var = var - vcl_pow(mean,2);

  gaussian.set_mean(mean);
  gaussian.set_var(var);

}
