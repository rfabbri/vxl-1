//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 05/01/09
//
//

#include "dbrec_image_models.h"
#include <bsta/bsta_gauss_f1.h>

float dbrec_gaussian_bg_appearance_model::prob_density(float s, unsigned i, unsigned j) const
{
  assert(i < mu_img_.ni());
  assert(j < mu_img_.nj());
  float mu_bk = mu_img_(i,j);
  float sigma_bk = sigma_img_(i,j);
  bsta_gauss_f1 pdbg(mu_bk, sigma_bk*sigma_bk);
  return pdbg.prob_density(s);
}

//: location variant prior model, e.g. when using a background map to interpret the strengths
float dbrec_loc_prior_model::prior(unsigned i, unsigned j) const 
{
  return prior_img_(i,j);
}
float dbrec_indep_prior_model::prior(unsigned i, unsigned j) const
{
  float product = 1.0f;
  for (unsigned ind = 0; ind < models_.size(); ind++) {
    product *= models_[ind]->prior(i,j);
  }
  return product;
}

