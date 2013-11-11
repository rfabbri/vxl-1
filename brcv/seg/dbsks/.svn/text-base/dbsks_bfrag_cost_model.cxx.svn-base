// This is file shp/dbsks/dbsks_bfrag_cost_model.cxx

//:
// \file

#include "dbsks_bfrag_cost_model.h"
#include <vnl/vnl_math.h>



//==============================================================================
// dbsks_bfrag_cost_model
//==============================================================================

//------------------------------------------------------------------------------
//: Constructor
dbsks_bfrag_cost_model::
dbsks_bfrag_cost_model()
{
  this->active_dist_type_[0] = this->active_dist_type_[1] = CONSTANT;
  this->constant_dist_[0] = this->constant_dist_[1] = 1;
}

//------------------------------------------------------------------------------
//: Probability-density for a given cost value
// cost_type = 0: foreground
// cost_type = 1: background
double dbsks_bfrag_cost_model::
prob_density(int cost_type, double x) const
{
  switch (this->active_dist_type_[cost_type])
  {
    case CONSTANT:
      return this->constant_dist_[cost_type];
    case WEIBULL:
      // force cost > 0 to prevent log(0)
      x = vnl_math_max(x, 1e-5);
      return this->weibull_dist_[cost_type].prob_density(x);
    case NONPARAM:
      return this->nonparam_dist_[cost_type].p(x);
    default:
      return 0;
  }
  return 0;
}


//------------------------------------------------------------------------------
//: Log likelihood ratio for foreground and background
double dbsks_bfrag_cost_model::
log_likelihood_ratio(double x) const
{
  double prob_fg = this->prob_density(FOREGROUND, x);
  double prob_bg = this->prob_density(BACKGROUND, x);
  
  // avoid log(0 / 0)
  return (prob_fg == prob_bg) ? 0 : vcl_log( prob_fg / prob_bg );
}

