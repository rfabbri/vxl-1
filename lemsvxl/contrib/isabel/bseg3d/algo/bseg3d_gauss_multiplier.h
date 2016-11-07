
// This is contrib/isabel/bseg3d/algo/bseg3d_multiply_mixtures.h
#ifndef bseg3d_gauss_multiplier_h_
#define bseg3d_gauss_multiplier_h_
//:
// \file
// \brief class to multiply gaussian pdfs
// \date April 20, 2009
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <vcl_utility.h>
class bseg3d_gauss_multiplier
{

private:

  typedef bsta_num_obs<bsta_gauss_f1> gauss_type;

public:

  //:Returns a pair containing normalization constant and a normalized gaussian
  // This is necessary because the product of gaussian pdfs is an unormalized gaussian pdf

   vcl_pair<double, gauss_type> multiply(vcl_vector<bsta_num_obs<bsta_gauss_f1> > const &gauss_vec);
};

#endif
