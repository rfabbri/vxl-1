// This is contrib/isabel/bseg3d/algo/bseg3d_gmm_l2norm.h
#ifndef bseg3d_gmm_l2norm_h_
#define bseg3d_gmm_l2norm_h_
//:
// \file
// \brief class to calculate the l2-norm between mixtures of gaussians(Three mixtures for now)
//        The derivation of the distance is a bseg3d/doc/L2_Disatnce.pdf
// \author Isabel Restrepo
// \date March 25, 2009
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gaussian_indep.h>

class bseg3d_gmm_l2norm
{

private:

  typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;
  
  typedef bsta_mixture<gauss_type> mix_gauss_undef;

public:

double distance (mix_gauss_type const& p1, mix_gauss_type const& p2);
float l2_gauss2mix(gauss_type const&g, mix_gauss_undef const& f, bool normalize);
float l2_gauss(gauss_type const& p1, gauss_type const& p2, bool normalize);
float kl_symmetric_distance(gauss_type const&g1, gauss_type const& g2);
float kl_distance(gauss_type const&g1, gauss_type const& g2);

};

#endif

