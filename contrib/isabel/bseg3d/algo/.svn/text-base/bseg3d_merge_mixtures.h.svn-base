// This is contrib/isabel/bseg3d/algo/bseg3d_merge_mixtures.h
#ifndef bseg3d_merge_mixtures_h_
#define bseg3d_merge_mixtures_h_
//:
// \file
// \brief class to merge gaussian mixtures. The merging equations are given in bseg3d/doc/diffRegions.pdf
// \author Isabel Restrepo
// \date April 20, 2009
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>

class bseg3d_merge_mixtures
{

private:

  typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;

public:
  //:Merges all the components in the mixture into one component
  void all_to_one(mix_gauss_type const& mixture,bsta_gauss_f1 &gaussian);

};

#endif

