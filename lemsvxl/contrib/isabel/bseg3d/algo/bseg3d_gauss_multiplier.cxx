#include "bseg3d_gauss_multiplier.h"

//:Returns a pair containing normalization constant and a normalized gaussian.
// This is necessary because the product of gaussian pdfs is an unormalized gaussian pdf
std::pair<double, bsta_num_obs<bsta_gauss_f1> > bseg3d_gauss_multiplier::multiply(std::vector<bsta_num_obs<bsta_gauss_f1> > const &gauss_vec)
{
  double mean = double(0);
  double var = double(0);
  double norm_const = double(0);

  std::vector<bsta_num_obs<bsta_gauss_f1> >::const_iterator vit = gauss_vec.begin();

  double sum_term = double(0);
  double prod_term = double(1);
  for(; vit!=gauss_vec.end(); vit++)
  {
    mean = mean + (*vit).mean()/((*vit).var());
    var = var + 1/(*vit).var(); 
    
    std::vector<bsta_num_obs<bsta_gauss_f1> >::const_iterator vit2 = (vit+1);
    for(; vit2!=gauss_vec.end(); vit2++)
    {
      sum_term = sum_term + std::pow((*vit).mean() - (*(vit2)).mean(),2)/
        ((*vit).var()*(*vit2).var());
    }
    prod_term = prod_term * 2*vnl_math::pi*(*vit).var();
  }

  var = (1/var);
  mean = mean * var;
  double t1 = std::sqrt((2*vnl_math::pi * var)/prod_term);
  double t2 = -(1/2)*var*sum_term;
  double t3 =std::exp(t2);
  norm_const = t1*t2 ;


  return std::make_pair(norm_const, bsta_gauss_f1(float(mean),float(var)));
}
