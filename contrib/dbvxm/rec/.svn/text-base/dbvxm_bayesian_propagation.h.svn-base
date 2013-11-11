//:
// \file
// \brief dbvxm Bayesian propagation algorithm to update probability maps using prior probability distributions on appearance
//
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/01/08
//      
// \verbatim
//   Modifications
//  
// \endverbatim
//
//


#if !defined(_dbvxm_bayesian_propagation_H)
#define _dbvxm_bayesian_propagation_H

#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vcl_string.h>

#include <vil/vil_image_view.h>

#include "dbvxm_fg_pair_density.h"
#include "dbvxm_fg_bg_pair_density.h"
#include "dbvxm_bg_pair_density.h"

class dbvxm_bayesian_propagation
{
public:
  dbvxm_bayesian_propagation(vil_image_view<vxl_byte>& inp, vil_image_view<float>& prob_density) : img_view_(inp), prob_density_(prob_density) { convert_img(); }

  bool initialize_bg_map(float high_prior = 0.9f, float top_percentile = 0.1f);

  bool horizontal_pass(dbvxm_fg_pair_density& fgp, dbvxm_fg_bg_pair_density& fgbgp, dbvxm_bg_pair_density& bgp);
  bool directional_pass(dbvxm_fg_pair_density& fgp, dbvxm_fg_bg_pair_density& fgbgp, dbvxm_bg_pair_density& bgp, int ii, int jj);

  bool horizontal_pass_using_prob_density_as_bgp(dbvxm_fg_pair_density& fgp, dbvxm_fg_bg_pair_density& fgbgp);
  bool vertical_pass_using_prob_density_as_bgp(dbvxm_fg_pair_density& fgp, dbvxm_fg_bg_pair_density& fgbgp);
  bool directional_pass_using_prob_density_as_bgp(dbvxm_fg_pair_density& fgp, dbvxm_fg_bg_pair_density& fgbgp, int ii, int jj);

  bool pass_using_prob_density_as_bgp(dbvxm_fg_pair_density& fgp, dbvxm_fg_bg_pair_density& fgbgp, vil_image_view<float>& temp);

  bool run(dbvxm_fg_pair_density& fgp, dbvxm_fg_bg_pair_density& fgbgp, dbvxm_bg_pair_density& bgp, int npasses = 1);
  bool run_using_prob_density_as_bgp(dbvxm_fg_pair_density& fgp, dbvxm_fg_bg_pair_density& fgbgp, int npasses = 1);

protected: 
  void convert_img() {
    ni_ = img_view_.ni();
    nj_ = img_view_.nj();
    img_.set_size(ni_, nj_, 1);
    vil_convert_stretch_range_limited(img_view_, img_, (vxl_byte)0, (vxl_byte)255, 0.0f, 1.0f);
  }

public:

  unsigned ni_;
  unsigned nj_;
  vil_image_view<float> img_;
  vil_image_view<vxl_byte> img_view_;
  vil_image_view<float> prob_density_;

  vil_image_view<float> bg_map_;  // foreground map is always inverse of this map, i.e. fg(i,j) = 1-bg_map(i,j)

};

#endif  //_dbvxm_bayesian_propagation_H
